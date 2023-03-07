module("luci.controller.admin.thread", package.seeall)

function index()
	page = entry({"admin", "network", "thread"}, template("admin_thread/thread_overview"), translate("Thread"), 16)
	page.leaf = true

	page = entry({"admin", "network", "thread_state"}, call("thread_state"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_neighbors"}, call("thread_neighbors"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_graph"}, call("thread_graph"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_scan"}, template("admin_thread/thread_scan"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_create"}, template("admin_thread/thread_setting"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_setting"}, template("admin_thread/thread_setting"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_add"}, post("thread_add"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_add_page"}, template("admin_thread/thread_add"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_view"}, template("admin_thread/thread_view"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_join"}, template("admin_thread/thread_join"), nil)
	page.leaf = true

	page = entry({"admin", "network", "joiner_remove"}, post("joiner_remove"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_attach"}, post("thread_attach"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_add_joiner"}, post("thread_add_joiner"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_handler_setting"}, post("thread_handler_setting"), nil)
	page.leaf = true

	page = entry({"admin", "network", "thread_stop"}, post("thread_stop"), nil)
	page.leaf = true
end

function thread_handler_setting()
	local ubus = require "ubus"
	local tpl = require "luci.template"
	local http = require "luci.http"
	local networkname = luci.http.formvalue("threadname")
	local channel = luci.http.formvalue("channel") + 0
	local panid = luci.http.formvalue("panid")
	local extpanid = luci.http.formvalue("extpanid")
	local mode = luci.http.formvalue("mode")
	local networkkey = luci.http.formvalue("networkkey")
	local pskc = luci.http.formvalue("pskc")
	local macfilter = luci.http.formvalue("macfilterselect")
	local macfilteradd = luci.http.formvalue("macfilteradd")
	local macfilterremove = luci.http.formvalue("macfilterremove")
	local submitcontent = luci.http.formvalue("submitcontent")
	local jumpflag = 0

	local vError = 0
	local result

	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	if submitcontent == "enable" then
		result = conn:call("otbr", "threadstart", {})
		vError = result.Error
	elseif submitcontent == "disable" then
		result = conn:call("otbr", "threadstop", {})
		vError = result.Error
	elseif submitcontent == "leave" then
		result = conn:call("otbr", "leave", {})
		vError = result.Error
	elseif submitcontent == "clearAddr" then
		result = conn:call("otbr", "macfilterclear", {})
		vError = result.Error
		jumpflag = 1
	elseif submitcontent == "addAddr" then
		result = conn:call("otbr", "macfilteradd", { addr = macfilteradd })
		conn:call("otbr", "macfiltersetstate", { state = macfilter })
		vError = result.Error
		jumpflag = 1
	elseif submitcontent == "removeAddr" then
		if type(macfilterremove) == "table" then
			local removeAddrIndex = luci.http.formvalue("removeAddrIndex") + 0
			result = conn:call("otbr", "macfilterremove", { addr = macfilterremove[removeAddrIndex] })
		else
			result = conn:call("otbr", "macfilterremove", { addr = macfilterremove })
		end
		conn:call("otbr", "macfiltersetstate", { state = macfilter })
		vError = result.Error
		jumpflag = 1
	else
		if(threadget("state").State == "disabled")then
			conn:call("otbr", "setnetworkname", { networkname = networkname })
			conn:call("otbr", "setchannel", { channel = channel })
			conn:call("otbr", "setpanid", { panid = panid })
			conn:call("otbr", "setextpanid", { extpanid = extpanid })
			conn:call("otbr", "setmode", { mode = mode })
			conn:call("otbr", "setnetworkkey", { networkkey = networkkey })
			conn:call("otbr", "setpskc", { pskc = pskc })
			conn:call("otbr", "macfiltersetstate", { state = macfilter })
			result = conn:call("otbr", "threadstart", {})
		else
			result = conn:call("otbr", "mgmtset", { networkkey = networkkey, networkname = networkname, extpanid = extpanid, panid = panid, channel = tostring(channel), pskc = pskc })
			conn:call("otbr", "macfiltersetstate", { state = macfilter })
		end
		vError = result.Error
	end

	if(jumpflag == 0) then
		local stat, dsp = pcall(require, "luci.dispatcher")
		stat = 0
		luci.http.redirect(stat and dsp.build_url("admin", "network", "thread") .. "?error=" .. vError)
	else
		local stat, dsp = pcall(require, "luci.dispatcher")
		luci.http.redirect(stat and dsp.build_url("admin", "network", "thread_setting") .. "?error=" .. vError)
	end
end

function thread_add()
	local ubus = require "ubus"
	local tpl = require "luci.template"
	local http = require "luci.http"

	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	result = conn:call("otbr", "commissionerstart", {})
	local vError = result.Error

	local stat, dsp = pcall(require, "luci.dispatcher")
	luci.http.redirect(stat and dsp.build_url("admin", "network", "thread_add_page") .. "?error=" .. vError)
end

function thread_add_joiner()
	local ubus = require "ubus"
	local tpl = require "luci.template"
	local http = require "luci.http"
	local pskd = luci.http.formvalue("pskd")
	local eui64 = luci.http.formvalue("eui64")

	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	result = conn:call("otbr", "joineradd", { pskd = pskd, eui64 = eui64})
	vError = result.Error

	local stat, dsp = pcall(require, "luci.dispatcher")
	luci.http.redirect(stat and dsp.build_url("admin", "network", "thread_view") .. "?error=" .. vError)
end

function joiner_remove()
	local ubus = require "ubus"
	local tpl = require "luci.template"
	local http = require "luci.http"
	local eui64 = luci.http.formvalue("eui64")

	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	result = conn:call("otbr", "joinerremove", { eui64 = eui64 })
	vError = result.Error

	local stat, dsp = pcall(require, "luci.dispatcher")
	luci.http.redirect(stat and dsp.build_url("admin", "network", "thread_view") .. "?error=" .. vError)
end

function thread_attach()
	local ubus = require "ubus"
	local tpl = require "luci.template"
	local http = require "luci.http"
	local panid = luci.http.formvalue("panid")
	local channel = luci.http.formvalue("channel") + 0
	local networkkey = luci.http.formvalue("networkkey")

	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	conn:call("otbr", "setpanid", { panid = panid })
	conn:call("otbr", "setchannel", { channel = channel })
	conn:call("otbr", "setnetworkkey", { networkkey = networkkey })
	result = conn:call("otbr", "threadstart", {})
	vError = result.Error

	local stat, dsp = pcall(require, "luci.dispatcher")
	luci.http.redirect(stat and dsp.build_url("admin", "network", "thread") .. "?error=" .. vError)
end

function thread_stop()
	local ubus = require "ubus"
	local tpl = require "luci.template"
	local http = require "luci.http"

	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	result = conn:call("otbr", "threadstop", {})
	vError = result.Error

	local stat, dsp = pcall(require, "luci.dispatcher")
	luci.http.redirect(stat and dsp.build_url("admin", "network", "thread") .. "?error=" .. vError)
end

function thread_join()
	local tpl = require "luci.template"
	local http = require "luci.http"

	local stat, dsp = pcall(require, "luci.dispatcher")
	luci.http.redirect(stat and dsp.build_url("admin", "network", "thread_join"))
end

function thread_state()
	luci.http.prepare_content("application/json")

	local result = {}
	result.state = threadget("state").State

	if(result.state ~= "disabled") then
		result.panid = threadget("panid").PanId
		result.channel = threadget("channel").Channel
		result.networkname = threadget("networkname").NetworkName
	end
	luci.http.write_json(result)
end

function thread_graph()
	luci.http.prepare_content("application/json")

	luci.http.write_json(networkdata())
end

function thread_neighbors()
	luci.http.prepare_content("application/json")

	local result = {}
	local neighbor = neighborlist()

	result.neighbor = neighbor.neighborlist

	local joiner = joinerlist()
	result.joinernum = joiner.joinernum
	result.joinerlist = joiner.joinerlist

	result.state = threadget("state").State

	luci.http.write_json(result)
end

function networkdata()
	local k, v, m, n
	local data = { }
	local l = { }

	local result = connect_ubus("networkdata")

	for k, v in pairs(result) do
		l[#l+1] = v
	end

	data.connect = l
	data.state = threadget("state").State
	data.rloc16 = threadget("rloc16").rloc16
	data.joinernum = threadget("joinernum").joinernum
	data.leader = threadget("leaderdata").leaderdata.LeaderRouterId
	return data
end

function joinerlist()
	local k, v, result
	local l = { }
	local data = { }

	result = connect_ubus("joinernum")
	data.joinernum = result.joinernum

	if result.Joinernum ~= 0 then
		for k, v in pairs(result.joinerList) do
			l[#l+1] = v
		end
	end

	data.joinerlist = l
	return data
end

function neighborlist()
	local k, v, result, tmpResult
	local l = { }
	local data = { }

	local state = threadget("state").State

	if state == 'child' then
		tmpResult = connect_ubus("parent")
		result = tmpResult.parent_list
	else
		tmpResult = connect_ubus("neighbor")
		result = tmpResult.neighbor_list
	end

	for k, v in pairs(result) do
		l[#l+1] = v
	end

	data.neighborlist = l
	return data
end

function connect_ubus(methods)
	local ubus = require "ubus"
	local result
	local conn = ubus.connect()

	if not conn then
		error("Failed to connect to ubusd")
	end

	result = conn:call("otbr", methods, {})

	return result
end

function threadget(action)
	local result = connect_ubus(action)

	return result
end
