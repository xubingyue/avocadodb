# coding: utf-8

require 'rspec'
require 'avocadodb.rb'

describe AvocadoDB do
  prefix = "api-async"

  context "dealing with async requests:" do

################################################################################
## checking methods 
################################################################################
    
    it "checks whether async=false returns status 202" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_get("#{prefix}-get-status", cmd, :headers => { "X-Avocado-Async" => "false" })

      doc.code.should eq(200)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should_not be_nil
    end

    it "checks whether async=true returns status 202" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_get("#{prefix}-get-status", cmd, :headers => { "X-Avocado-Async" => "true" })

      doc.code.should eq(202)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should eq ""
    end
    
    it "checks whether async=1 returns status 202" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_get("#{prefix}-get-status", cmd, :headers => { "X-Avocado-Async" => "1" })

      doc.code.should eq(200)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should_not be_nil
    end
    
    it "checks whether HEAD returns status 202" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_head("#{prefix}-head-status", cmd, :headers => { "X-Avocado-Async" => "true" })

      doc.code.should eq(202)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should be_nil
    end
    
    it "checks whether POST returns status 202" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_post("#{prefix}-head-status", cmd, :body => "", :headers => { "X-Avocado-Async" => "true" })

      doc.code.should eq(202)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should eq ""
    end
    
    it "checks whether an invalid location returns status 202" do
      cmd = "/_api/not-existing"
      doc = AvocadoDB.log_get("#{prefix}-get-non-existing", cmd, :headers => { "X-Avocado-Async" => "true" })

      doc.code.should eq(202)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should eq ""
    end
    
    it "checks whether a failing action returns status 202" do
      cmd = "/_admin/execute"
      body = "fail();"
      doc = AvocadoDB.log_post("#{prefix}-post-failing", cmd, :body => body, :headers => { "X-Avocado-Async" => "true" })

      doc.code.should eq(202)
      doc.headers.should_not have_key("x-avocado-async-id")
      doc.response.body.should eq ""
    end
    
    it "checks the responses when the queue fills up" do
      cmd = "/_api/version"

      (1..500).each do
        doc = AvocadoDB.log_get("#{prefix}-get-queue", cmd, :headers => { "X-Avocado-Async" => "true" })

        doc.code.should eq(202)
        doc.headers.should_not have_key("x-avocado-async-id")
        doc.response.body.should eq ""
      end
    end

    it "checks whether setting x-avocado-async to 'store' returns a job id" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_get("#{prefix}-get-check-id", cmd, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      doc.headers["x-avocado-async-id"].should match(/^\d+$/)
      doc.response.body.should eq ""
    end
    
    it "checks whether job api returns 200 for ready job" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-200", cmd, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""
     
      sleep 2
 
      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-200", cmd)
      doc.code.should eq(200)
      doc.response.body.should eq ""

      # should be able to query the status again
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-200", cmd)
      doc.code.should eq(200)
      doc.response.body.should eq ""

      # should also be able to fetch the result of the job
      doc = AvocadoDB.log_put("#{prefix}-get-check-status-200", cmd)
      doc.code.should eq(200)
      doc.parsed_response["server"].should eq "avocado"

      # now it should be gone
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-200", cmd)
      doc.code.should eq(404)
    end

    it "checks whether job api returns 204 for non-ready job" do
      cmd = "/_admin/sleep?duration=5"
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-204", cmd, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""
      
      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-204", cmd)
      doc.code.should eq(204)
      doc.response.body.should be_nil
    end

    it "checks whether job api returns 404 for non-existing job" do
      cmd = "/_api/version"
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-404", cmd, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""
 
      cmd = "/_api/job/123" + id + "123" # assume this id is invalid
      doc = AvocadoDB.log_get("#{prefix}-get-check-status-404", cmd)
      doc.code.should eq(404)
    end

    it "checks whether job api returns done job in pending and done list" do
      cmd = "/_admin/sleep?duration=5"
      doc = AvocadoDB.log_get("#{prefix}-get-check-done", cmd, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""
      
      cmd = "/_api/job/done"
      doc = AvocadoDB.log_get("#{prefix}-get-check-done", cmd)
      doc.code.should eq(200)
      doc.parsed_response.should_not include(id)
      
      cmd = "/_api/job/pending"
      doc = AvocadoDB.log_get("#{prefix}-get-check-done", cmd)
      doc.code.should eq(200)
      doc.parsed_response.should include(id)
    end

    it "checks whether job api returns non-ready job in pending and done lists" do
      cmd = "/_admin/sleep?duration=5"
      doc = AvocadoDB.log_get("#{prefix}-get-check-pending", cmd, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""
      
      cmd = "/_api/job/pending"
      doc = AvocadoDB.log_get("#{prefix}-get-check-pending", cmd)
      doc.code.should eq(200)
      doc.parsed_response.should include(id)
      
      cmd = "/_api/job/done"
      doc = AvocadoDB.log_get("#{prefix}-get-check-pending", cmd)
      doc.code.should eq(200)
      doc.parsed_response.should_not include(id)
    end
    
    it "checks whether we can cancel an AQL query" do
      cmd = "/_api/cursor"
      body = '{"query": "for x in 1..1000 let a = sleep(0.5) filter x == 1 return x"}'
      doc = AvocadoDB.log_post("#{prefix}-create-cursor", cmd, :body => body, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-check-status-204", cmd)
      doc.code.should eq(204)

      cmd = "/_api/job/" + id + "/cancel"
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-cancel", cmd)
      doc.code.should eq(200)

      sleep 5

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-check-status-410", cmd)
      doc.code.should eq(410)
    end
    
    it "checks whether we can cancel an AQL query" do
      cmd = "/_api/cursor"
      body = '{"query": "for x in 1..10000 for y in 1..10000 let a = sleep(0.01) filter x == 1 && y == 1 return x"}'
      doc = AvocadoDB.log_post("#{prefix}-create-cursor", cmd, :body => body, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-check-status-204", cmd)
      doc.code.should eq(204)

      sleep 2

      cmd = "/_api/job/" + id + "/cancel"
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-cancel", cmd)
      doc.code.should eq(200)

      sleep 5

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-check-status-410", cmd)
      doc.code.should eq(410)
    end

    it "checks whether we can cancel an AQL query" do
      cmd = "/_api/cursor"
      body = '{"query": "for x in 1..10000 for y in 1..10000 let a = sleep(0.01) filter x == 1 && y == 1 return x"}'
      doc = AvocadoDB.log_post("#{prefix}-create-cursor", cmd, :body => body, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-check-status-204", cmd)
      doc.code.should eq(204)

      cmd = "/_api/job/" + id + "/cancel"
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-cancel", cmd)
      doc.code.should eq(200)

      sleep 5

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-cursor-check-status-410", cmd)
      doc.code.should eq(410)
    end

    it "checks whether we can cancel a transaction" do
      cmd = "/_api/transaction"
      body = '{"collections": {"write": "_graphs"}, "action": "function() {var i = 0; while (i < 90000000000) {i++; require(\\"internal\\").wait(0.1); } return i;}"}'
      doc = AvocadoDB.log_post("#{prefix}-create-transaction", cmd, :body => body, :headers => { "X-Avocado-Async" => "store" })

      doc.code.should eq(202)
      doc.headers.should have_key("x-avocado-async-id")
      id = doc.headers["x-avocado-async-id"]
      id.should match(/^\d+$/)
      doc.response.body.should eq ""

      sleep 1

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-transaction-check-status-204", cmd)
      doc.code.should eq(204)

      sleep 1

      cmd = "/_api/job/" + id + "/cancel"
      doc = AvocadoDB.log_put("#{prefix}-create-transaction-cancel", cmd)
      doc.code.should eq(200)

      sleep 5

      cmd = "/_api/job/" + id
      doc = AvocadoDB.log_put("#{prefix}-create-transaction-check-status-410", cmd)
      doc.code.should eq(410)
    end
  end
end
