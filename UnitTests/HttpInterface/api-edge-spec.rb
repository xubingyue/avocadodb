# coding: utf-8

require 'rspec'
require 'uri'
require 'avocadodb.rb'

describe AvocadoDB do
  prefix = "rest-edge"

  context "creating an edge:" do

################################################################################
## error handling
################################################################################

    context "error handling:" do
      it "returns an error if collection does not exist" do
        cn = "UnitTestsCollectionEdge"
        AvocadoDB.drop_collection(cn)
        cmd = "/_api/document?collection=#{cn}"
        body = "{\"_from\":\"test\",\"_to\":\"test\"}"
        doc = AvocadoDB.log_post("#{prefix}-no-collection", cmd, :body => body)

        doc.code.should eq(404)
        doc.parsed_response['error'].should eq(true)
        doc.parsed_response['errorNum'].should eq(1203)
        doc.parsed_response['code'].should eq(404)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end
    end

################################################################################
## known collection name
################################################################################

    context "known collection name:" do
      before do
        @ce = "UnitTestsCollectionEdge"
        @eid = AvocadoDB.create_collection(@ce, true, 3) # type 3 = edge collection
        @cv = "UnitTestsCollectionVertex"
        @vid = AvocadoDB.create_collection(@cv, true, 2) # type 2 = document collection
    
        @reFull = Regexp.new('^[a-zA-Z0-9_\-]+/\d+$')
        @reEdge = Regexp.new('^' + @ce + '/')
        @reVertex = Regexp.new('^' + @cv + '/')
      end

      after do
        AvocadoDB.drop_collection(@ce)
        AvocadoDB.drop_collection(@cv)
      end

      it "creating an edge" do
        cmd = "/_api/document?collection=#{@cv}"

        # create first vertex
        body = "{ \"a\" : 1 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reVertex)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id1 = doc.parsed_response['_id']

        # create second vertex
        body = "{ \"a\" : 2 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reVertex)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id2 = doc.parsed_response['_id']

        # create edge
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{id1}\" , \"_to\" : \"#{id2}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id3 = doc.parsed_response['_id']

        # check edge
        cmd = "/_api/document/#{id3}"
        doc = AvocadoDB.log_get("#{prefix}-read-edge", cmd)

        doc.code.should eq(200)
        doc.parsed_response['_id'].should eq(id3)
        doc.parsed_response['_from'].should eq(id1)
        doc.parsed_response['_to'].should eq(id2)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
        
        # create another edge
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"e\" : 1, \"_from\" : \"#{id1}\", \"_to\" : \"#{id2}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id4 = doc.parsed_response['_id']

        # check edge
        cmd = "/_api/document/#{id4}"
        doc = AvocadoDB.log_get("#{prefix}-read-edge", cmd)

        doc.code.should eq(200)
        doc.parsed_response['_id'].should eq(id4)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.parsed_response['_from'].should eq(id1)
        doc.parsed_response['_from'].should match(@reFull)
        doc.parsed_response['_from'].should match(@reVertex)
        doc.parsed_response['_to'].should eq(id2)
        doc.parsed_response['_to'].should match(@reFull)
        doc.parsed_response['_to'].should match(@reVertex)
        doc.parsed_response['e'].should eq(1)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        # create third edge
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"e\" : 2, \"_from\" : \"#{id2}\", \"_to\" : \"#{id1}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id5 = doc.parsed_response['_id']

        # check edge
        cmd = "/_api/document/#{id5}"
        doc = AvocadoDB.log_get("#{prefix}-read-edge", cmd)

        doc.code.should eq(200)
        doc.parsed_response['_id'].should eq(id5)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.parsed_response['_from'].should eq(id2)
        doc.parsed_response['_from'].should match(@reFull)
        doc.parsed_response['_from'].should match(@reVertex)
        doc.parsed_response['_to'].should eq(id1)
        doc.parsed_response['_to'].should match(@reFull)
        doc.parsed_response['_to'].should match(@reVertex)
        doc.parsed_response['e'].should eq(2)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end

      it "using collection names in from and to" do
        cmd = "/_api/document?collection=#{@cv}"

        # create a vertex
        body = "{ \"a\" : 1 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-named", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reVertex)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id = doc.parsed_response['_id']
        # replace collection id with collection name
        translated = URI.escape(id.gsub /^\d+\//, 'UnitTestsCollectionVertex/')
  
        # create edge, using collection id
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{id}\", \"_to\" : \"#{id}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-named", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        # create edge, using collection names
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{translated}\", \"_to\" : \"#{translated}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-named", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
              
        # create edge, using mix of collection names and ids
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{translated}\", \"_to\" : \"#{id}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-named", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
  
        # turn parameters around
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{id}\", \"_to\" : \"#{translated}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-named", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end
            
      it "using collection ids in collection, from and to" do
        cmd = "/_api/document?collection=#{@vid}"

        # create a vertex
        body = "{ \"a\" : 1 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-id", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reVertex)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id = doc.parsed_response['_id']
        # replace collection name with collection id
        translated = URI.escape(id.gsub /UnitTestsCollectionVertex/, @vid)
  
        # create edge, using collection id
        cmd = "/_api/document?collection=#{@eid}"
        body = "{ \"_from\" : \"#{id}\", \"_to\" : \"#{id}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-named", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should match(@reFull)
        doc.parsed_response['_id'].should match(@reEdge)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end

################################################################################
## create an edge using keys
################################################################################

      it "creating an edge using keys" do
        cmd = "/_api/document?collection=#{@cv}"

        # create first vertex
        body = "{ \"name\" : \"Fred\", \"_key\" : \"fred\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should eq("#{@cv}/fred")
        doc.parsed_response['_key'].should be_kind_of(String)
        doc.parsed_response['_key'].should eq("fred")
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id1 = doc.parsed_response['_id']

        # create second vertex
        body = "{ \"name\" : \"John\", \"_key\" : \"john\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should eq("#{@cv}/john")
        doc.parsed_response['_key'].should be_kind_of(String)
        doc.parsed_response['_key'].should eq("john")
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id2 = doc.parsed_response['_id']

        # create edge
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{id1}\", \"_to\" : \"#{id2}\", \"what\" : \"fred->john\", \"_key\" : \"edge1\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should eq("#{@ce}/edge1")
        doc.parsed_response['_key'].should be_kind_of(String)
        doc.parsed_response['_key'].should eq("edge1")
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id3 = doc.parsed_response['_id']

        # check edge
        cmd = "/_api/document/#{id3}"
        doc = AvocadoDB.log_get("#{prefix}-create-edge-key", cmd)

        doc.code.should eq(200)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.parsed_response['_id'].should eq(id3)
        doc.parsed_response['_key'].should be_kind_of(String)
        doc.parsed_response['_key'].should eq("edge1")
        doc.parsed_response['_from'].should eq(id1)
        doc.parsed_response['_to'].should eq(id2)
        doc.parsed_response['what'].should eq("fred->john")
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end

################################################################################
## create using invalid keys
################################################################################

      it "creating an edge using invalid keys" do
        cmd = "/_api/document?collection=#{@cv}"

        # create first vertex
        body = "{ \"name\" : \"Fred\", \"_key\" : \"fred\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key-invalid", cmd, :body => body)

        doc.code.should eq(201)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id1 = doc.parsed_response['_id']

        # create second vertex
        body = "{ \"name\" : \"John\", \"_key\" : \"john\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key-invalid", cmd, :body => body)

        doc.code.should eq(201)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id2 = doc.parsed_response['_id']

        # create edge, 1st try
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{@cv}/rak/ov\", \"_to\" : \"#{@cv}/pj/otr\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key-invalid", cmd, :body => body)

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true)
        doc.parsed_response['errorNum'].should eq(1233)
        doc.parsed_response['code'].should eq(400)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
        
        # create edge, 2nd try
        cmd = "/_api/document?collection=#{@ce}"
        body = "{ \"_from\" : \"#{@cv}/rakov\", \"_to\" : \"#{@cv}/pjotr\", \"_key\" : \"the fox\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge-key-invalid", cmd, :body => body)

        doc.code.should eq(400)
        doc.parsed_response['error'].should eq(true)
        doc.parsed_response['errorNum'].should eq(1221)
        doc.parsed_response['code'].should eq(400)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end

    end

################################################################################
## known collection name, waitForSync URL param
################################################################################

    context "known collection name:" do
      before do
        @ce = "UnitTestsCollectionEdge"
        @eid = AvocadoDB.create_collection(@ce, false, 3) # type 3 = edge collection
        @cv = "UnitTestsCollectionVertex"
        @vid = AvocadoDB.create_collection(@cv, true, 2) # type 2 = document collection
      end

      after do
        AvocadoDB.drop_collection(@ce)
        AvocadoDB.drop_collection(@cv)
      end
      
      it "creating an edge, waitForSync URL param=false" do
        # create first vertex
        cmd = "/_api/document?collection=#{@cv}"
        body = "{ \"a\" : 1 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id1 = doc.parsed_response['_id']

        # create second vertex
        body = "{ \"a\" : 2 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id2 = doc.parsed_response['_id']

        # create edge
        cmd = "/_api/document?collection=#{@ce}&waitForSync=false"
        body = "{ \"_from\" : \"#{id1}\", \"_to\" : \"#{id2}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(202)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id3 = doc.parsed_response['_id']

        # check edge
        cmd = "/_api/document/#{id3}"
        doc = AvocadoDB.log_get("#{prefix}-read-edge", cmd)

        doc.code.should eq(200)
        doc.parsed_response['_id'].should eq(id3)
        doc.parsed_response['_from'].should eq(id1)
        doc.parsed_response['_to'].should eq(id2)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end
      
      it "creating an edge, waitForSync URL param=true" do
        # create first vertex
        cmd = "/_api/document?collection=#{@cv}"
        body = "{ \"a\" : 1 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id1 = doc.parsed_response['_id']

        # create second vertex
        body = "{ \"a\" : 2 }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id2 = doc.parsed_response['_id']

        # create edge
        cmd = "/_api/document?collection=#{@ce}&waitForSync=true"
        body = "{ \"_from\" : \"#{id1}\", \"_to\" : \"#{id2}\" }"
        doc = AvocadoDB.log_post("#{prefix}-create-edge", cmd, :body => body)

        doc.code.should eq(201)
        doc.parsed_response['_id'].should be_kind_of(String)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")

        id3 = doc.parsed_response['_id']

        # check edge
        cmd = "/_api/document/#{id3}"
        doc = AvocadoDB.log_get("#{prefix}-read-edge", cmd)

        doc.code.should eq(200)
        doc.parsed_response['_id'].should eq(id3)
        doc.parsed_response['_from'].should eq(id1)
        doc.parsed_response['_to'].should eq(id2)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
      end
    end

  end
end

