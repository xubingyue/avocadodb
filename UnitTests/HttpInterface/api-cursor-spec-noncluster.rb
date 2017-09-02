# coding: utf-8

require 'rspec'
require 'avocadodb.rb'

describe AvocadoDB do
  api = "/_api/cursor"
  prefix = "api-cursor"

################################################################################
## query cache
################################################################################

  context "testing the query cache:" do
    before do
      doc = AvocadoDB.get("/_api/query-cache/properties")
      @mode = doc.parsed_response['mode']
      AvocadoDB.put("/_api/query-cache/properties", :body => "{ \"mode\" : \"demand\" }")

      AvocadoDB.delete("/_api/query-cache")
    end

    after do
      AvocadoDB.put("/_api/query-cache/properties", :body => "{ \"mode\" : \"#{@mode}\" }")
    end

    it "testing without cache attribute set" do
      cmd = api
      body = "{ \"query\" : \"FOR i IN 1..5 RETURN i\" }"
      doc = AvocadoDB.log_post("#{prefix}-query-cache-disabled", cmd, :body => body)
      
      doc.code.should eq(201)
      doc.headers['content-type'].should eq("application/json; charset=utf-8")
      doc.parsed_response['error'].should eq(false)
      doc.parsed_response['code'].should eq(201)
      doc.parsed_response['id'].should be_nil
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')

      # should see same result, but not from cache
      doc = AvocadoDB.log_post("#{prefix}-query-cache", cmd, :body => body)
      doc.code.should eq(201)
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')
    end

    it "testing explicitly disabled cache" do
      cmd = api
      body = "{ \"query\" : \"FOR i IN 1..5 RETURN i\", \"cache\" : false }"
      doc = AvocadoDB.log_post("#{prefix}-query-cache-disabled", cmd, :body => body)
      
      doc.code.should eq(201)
      doc.headers['content-type'].should eq("application/json; charset=utf-8")
      doc.parsed_response['error'].should eq(false)
      doc.parsed_response['code'].should eq(201)
      doc.parsed_response['id'].should be_nil
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')

      # should see same result, but not from cache
      doc = AvocadoDB.log_post("#{prefix}-query-cache", cmd, :body => body)
      doc.code.should eq(201)
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')
    end

    it "testing enabled cache" do
      cmd = api
      body = "{ \"query\" : \"FOR i IN 1..5 RETURN i\", \"cache\" : true }"
      doc = AvocadoDB.log_post("#{prefix}-query-cache-enabled", cmd, :body => body)
      
      doc.code.should eq(201)
      doc.headers['content-type'].should eq("application/json; charset=utf-8")
      doc.parsed_response['error'].should eq(false)
      doc.parsed_response['code'].should eq(201)
      doc.parsed_response['id'].should be_nil
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')

      # should see same result, but now from cache
      doc = AvocadoDB.log_post("#{prefix}-query-cache", cmd, :body => body)
      doc.code.should eq(201)
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(true)
      doc.parsed_response['extra'].should_not have_key('stats')
    end

    it "testing clearing the cache" do
      cmd = api
      body = "{ \"query\" : \"FOR i IN 1..5 RETURN i\", \"cache\" : true }"
      doc = AvocadoDB.log_post("#{prefix}-query-cache-enabled", cmd, :body => body)
      
      doc.code.should eq(201)
      doc.headers['content-type'].should eq("application/json; charset=utf-8")
      doc.parsed_response['error'].should eq(false)
      doc.parsed_response['code'].should eq(201)
      doc.parsed_response['id'].should be_nil
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')

      # should see same result, but now from cache
      doc = AvocadoDB.log_post("#{prefix}-query-cache", cmd, :body => body)
      doc.code.should eq(201)
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(true)
      doc.parsed_response['extra'].should_not have_key('stats')

      # now clear cache
      AvocadoDB.delete("/_api/query-cache")

      # query again. now response should not come from cache
      doc = AvocadoDB.log_post("#{prefix}-query-cache", cmd, :body => body)
      doc.code.should eq(201)
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(false)
      doc.parsed_response['extra'].should have_key('stats')

      doc = AvocadoDB.log_post("#{prefix}-query-cache", cmd, :body => body)
      doc.code.should eq(201)
      result = doc.parsed_response['result']
      result.should eq([ 1, 2, 3, 4, 5 ])
      doc.parsed_response['cached'].should eq(true)
      doc.parsed_response['extra'].should_not have_key('stats')
    end

  end
end
