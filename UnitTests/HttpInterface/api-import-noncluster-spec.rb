# coding: utf-8

require 'rspec'
require 'avocadodb.rb'

describe AvocadoDB do
  api = "/_api/import"
  prefix = "api-import"

  context "importing documents:" do

################################################################################
## import with complete=true
################################################################################

    context "import with complete=true" do
      before do
        @cn = "UnitTestsImport"
        AvocadoDB.drop_collection(@cn)
        @cid = AvocadoDB.create_collection(@cn, false)
      end

      after do
        AvocadoDB.drop_collection(@cn)
      end

      it "regular" do
        cmd = api + "?collection=#{@cn}&type=auto&details=true&complete=true"
        body = "[{\"_key\":\"abc\",\"value1\":25,\"value2\":\"test\",\"allowed\":true},{\"_key\":\"abc\",\"name\":\"baz\"},{\"name\":{\"detailed\":\"detailed name\",\"short\":\"short name\"}}]"

        doc = AvocadoDB.log_post("#{prefix}-data-complete", cmd, :body => body)

        doc.code.should eq(409)
        doc.parsed_response['error'].should eq(true)
        doc.parsed_response['errorNum'].should eq(1210)
        
        AvocadoDB.size_collection(@cn).should eq(0)
      end
    end

  end
end
