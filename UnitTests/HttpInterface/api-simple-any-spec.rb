# coding: utf-8

require 'rspec'
require 'avocadodb.rb'

describe AvocadoDB do
  api = "/_api/simple"
  prefix = "api-simple"

  context "simple queries:" do

################################################################################
## any query
################################################################################

    context "any query:" do
      before do
        @cn = "UnitTestsCollectionSimple"
        AvocadoDB.drop_collection(@cn)
        
        body = "{ \"name\" : \"#{@cn}\", \"numberOfShards\" : 8 }"
        doc = AvocadoDB.post("/_api/collection", :body => body)
        doc.code.should eq(200)
        @cid = doc.parsed_response['id']
      end

      after do
        AvocadoDB.drop_collection(@cn)
      end
      
      it "get any document, wrong collection" do
        cmd = api + "/any"
        body = "{ \"collection\" : \"NonExistingCollection\" }"
        doc = AvocadoDB.log_put("#{prefix}-any", cmd, :body => body)

        doc.code.should eq(404)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
        doc.parsed_response['error'].should eq(true)
        doc.parsed_response['code'].should eq(404)
        doc.parsed_response['errorNum'].should eq(1203)
      end
      
      it "get any document, empty collection" do
        cmd = api + "/any"
        body = "{ \"collection\" : \"#{@cn}\" }"
        doc = AvocadoDB.log_put("#{prefix}-any", cmd, :body => body)

        doc.code.should eq(200)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
        doc.parsed_response['document'].should be_nil
      end
      
      it "get any document, single-document collection" do
        AvocadoDB.post("/_api/document?collection=#{@cn}", :body => "{ \"n\" : 30 }")

        cmd = api + "/any"
        body = "{ \"collection\" : \"#{@cn}\" }"
        doc = AvocadoDB.log_put("#{prefix}-any", cmd, :body => body)

        doc.code.should eq(200)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
        doc.parsed_response['error'].should eq(false)
        doc.parsed_response['code'].should eq(200)
        doc.parsed_response['document']['n'].should eq(30)
      end

      it "get any document, non-empty collection" do
        (0...10).each{|i|
          AvocadoDB.post("/_api/document?collection=#{@cn}", :body => "{ \"n\" : #{i} }")
        }

        cmd = api + "/any"
        body = "{ \"collection\" : \"#{@cn}\" }"
        doc = AvocadoDB.log_put("#{prefix}-any", cmd, :body => body)

        doc.code.should eq(200)
        doc.headers['content-type'].should eq("application/json; charset=utf-8")
        doc.parsed_response['error'].should eq(false)
        doc.parsed_response['code'].should eq(200)
        doc.parsed_response['document']['n'].should be_kind_of(Integer)
      end
    end

  end
end
