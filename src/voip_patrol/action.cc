/*
 * Copyright (C) 2016-2018 Julien Chavanton <jchavanton@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA~
 */

#include "voip_patrol.hh"
#include "action.hh"
#include "util.hh"
#include "string.h"
#include <pjsua2/presence.hpp>

void filter_accountname(std::string *str) {
	size_t index = 0;
	while (true) {
		index = str->find("@", index);
		if (index == std::string::npos) break;
		str->replace(index, 1, "_");
		index += 1;
	}
	index = 0;
	while (true) {
		index = str->find(";", index);
		if (index == std::string::npos) break;
		str->replace(index, 1, "-");
		index += 1;
	}
}

Action::Action(Config *cfg) : config{cfg} {
	init_actions_params();
	std::cout<<"Prepared for Action!\n";
}

vector<ActionParam> Action::get_params(string name) {
	if (name.compare("call") == 0) return do_call_params;
	else if (name.compare("register") == 0) return do_register_params;
	else if (name.compare("wait") == 0) return do_wait_params;
	else if (name.compare("accept") == 0) return do_accept_params;
	else if (name.compare("alert") == 0) return do_alert_params;
	else if (name.compare("codec") == 0) return do_codec_params;
	else if (name.compare("turn") == 0) return do_turn_params;
	else if (name.compare("message") == 0) return do_message_params;
	else if (name.compare("accept_message") == 0) return do_accept_message_params;
	vector<ActionParam> empty_params;
	return empty_params;
}

string Action::get_env(string env) {
	if (const char* val = std::getenv(env.c_str())) {
		std::string s(val);
		return s;
	} else {
		return "";
	}
}

bool Action::set_param(ActionParam &param, const char *val) {
	if (!val) return false;
	LOG(logINFO) << __FUNCTION__ << " param name:" << param.name << " val:" << val;
	if (param.type == APType::apt_bool) {
		if( strcmp(val, "false") ==  0 )  param.b_val = false;
		else param.b_val = true;
	} else if (param.type == APType::apt_integer) {
		param.i_val = atoi(val);
	} else if (param.type == APType::apt_float) {
		param.f_val = atof(val);
	} else {
		param.s_val = val;
		if (param.s_val.compare(0, 7, "VP_ENV_") == 0) {
			LOG(logINFO) << __FUNCTION__ << ": "<<param.name<<" "<<param.s_val<<" substitution:"<<get_env(val);
			param.s_val = get_env(val);
		}
	}
	return true;
}

bool Action::set_param_by_name(vector<ActionParam> *params, const string& name, const char *val) {
	for (auto &param : *params) {
		if (param.name.compare(name) == 0) {
				return set_param(param, val);
		}
	}
	return false;
}

void Action::init_actions_params() {
	// do_call
	do_call_params.push_back(ActionParam("caller", true, APType::apt_string));
	do_call_params.push_back(ActionParam("from", true, APType::apt_string));
	do_call_params.push_back(ActionParam("callee", true, APType::apt_string));
	do_call_params.push_back(ActionParam("to_uri", true, APType::apt_string));
	do_call_params.push_back(ActionParam("label", false, APType::apt_string));
	do_call_params.push_back(ActionParam("username", false, APType::apt_string));
	do_call_params.push_back(ActionParam("auth_username", false, APType::apt_string));
	do_call_params.push_back(ActionParam("password", false, APType::apt_string));
	do_call_params.push_back(ActionParam("realm", false, APType::apt_string));
	do_call_params.push_back(ActionParam("transport", false, APType::apt_string));
	do_call_params.push_back(ActionParam("expected_cause_code", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("wait_until", false, APType::apt_string));
	do_call_params.push_back(ActionParam("max_duration", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("repeat", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("max_ring_duration", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("min_mos", false, APType::apt_float));
	do_call_params.push_back(ActionParam("rtp_stats", false, APType::apt_bool));
	do_call_params.push_back(ActionParam("late_start", false, APType::apt_bool));
	do_call_params.push_back(ActionParam("srtp", false, APType::apt_string));
	do_call_params.push_back(ActionParam("force_contact", false, APType::apt_string));
	do_call_params.push_back(ActionParam("hangup", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("cancel", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("re_invite_interval", false, APType::apt_integer));
	do_call_params.push_back(ActionParam("play", false, APType::apt_string));
	do_call_params.push_back(ActionParam("record", false, APType::apt_string));
	do_call_params.push_back(ActionParam("play_dtmf", false, APType::apt_string));
	do_call_params.push_back(ActionParam("timer", false, APType::apt_string));
	do_call_params.push_back(ActionParam("proxy", false, APType::apt_string));
	// do_register
	do_register_params.push_back(ActionParam("transport", false, APType::apt_string));
	do_register_params.push_back(ActionParam("label", false, APType::apt_string));
	do_register_params.push_back(ActionParam("registrar", false, APType::apt_string));
	do_register_params.push_back(ActionParam("proxy", false, APType::apt_string));
	do_register_params.push_back(ActionParam("realm", false, APType::apt_string));
	do_register_params.push_back(ActionParam("username", false, APType::apt_string));
	do_register_params.push_back(ActionParam("auth_username", false, APType::apt_string));
	do_register_params.push_back(ActionParam("account", false, APType::apt_string));
	do_register_params.push_back(ActionParam("aor", false, APType::apt_string));
	do_register_params.push_back(ActionParam("password", false, APType::apt_string));
	do_register_params.push_back(ActionParam("unregister", false, APType::apt_bool));
	do_register_params.push_back(ActionParam("expected_cause_code", false, APType::apt_integer));
	do_register_params.push_back(ActionParam("reg_id", false, APType::apt_string));
	do_register_params.push_back(ActionParam("instance_id", false, APType::apt_string));
	do_register_params.push_back(ActionParam("srtp", false, APType::apt_string));
	do_register_params.push_back(ActionParam("rewrite_contact", true, APType::apt_bool));
	// do_accept
	do_accept_params.push_back(ActionParam("match_account", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("transport", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("label", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("cancel", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("max_duration", false, APType::apt_integer));
	do_accept_params.push_back(ActionParam("ring_duration", false, APType::apt_integer));
	do_accept_params.push_back(ActionParam("response_delay", false, APType::apt_integer));
	do_accept_params.push_back(ActionParam("early_media", false, APType::apt_bool));
	do_accept_params.push_back(ActionParam("wait_until", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("hangup", false, APType::apt_integer));
	do_accept_params.push_back(ActionParam("re_invite_interval", false, APType::apt_integer));
	//do_accept_params.push_back(ActionParam("min_mos", false, APType::apt_float));
	do_accept_params.push_back(ActionParam("rtp_stats", false, APType::apt_bool));
	do_accept_params.push_back(ActionParam("late_start", false, APType::apt_bool));
	do_accept_params.push_back(ActionParam("srtp", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("force_contact", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("play", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("record", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("code", false, APType::apt_integer));
	do_accept_params.push_back(ActionParam("call_count", false, APType::apt_integer));
	do_accept_params.push_back(ActionParam("reason", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("play_dtmf", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("timer", false, APType::apt_string));
	do_accept_params.push_back(ActionParam("fail_on_accept", false, APType::apt_bool));
	do_accept_params.push_back(ActionParam("expected_cause_code", false, APType::apt_integer));
	// do_wait
	do_wait_params.push_back(ActionParam("ms", false, APType::apt_integer));
	do_wait_params.push_back(ActionParam("complete", false, APType::apt_bool));
	// do_alert
	do_alert_params.push_back(ActionParam("email", false, APType::apt_string));
	do_alert_params.push_back(ActionParam("email_from", false, APType::apt_string));
	do_alert_params.push_back(ActionParam("smtp_host", false, APType::apt_string));
	// do_codec
	do_codec_params.push_back(ActionParam("priority", false, APType::apt_integer));
	do_codec_params.push_back(ActionParam("enable", false, APType::apt_string));
	do_codec_params.push_back(ActionParam("disable", false, APType::apt_string));
	// do_turn
	do_turn_params.push_back(ActionParam("enabled", false, APType::apt_bool));
	do_turn_params.push_back(ActionParam("server", false, APType::apt_string));
	do_turn_params.push_back(ActionParam("username", false, APType::apt_string));
	do_turn_params.push_back(ActionParam("password", false, APType::apt_string));
	do_turn_params.push_back(ActionParam("password_hashed", false, APType::apt_bool));
	do_turn_params.push_back(ActionParam("stun_only", false, APType::apt_bool));
	do_turn_params.push_back(ActionParam("sip_stun_use", false, APType::apt_bool));
	do_turn_params.push_back(ActionParam("media_stun_use", false, APType::apt_bool));
	// do_message
	do_message_params.push_back(ActionParam("from", true, APType::apt_string));
	do_message_params.push_back(ActionParam("to_uri", true, APType::apt_string));
	do_message_params.push_back(ActionParam("text", true, APType::apt_string));
	do_message_params.push_back(ActionParam("username", true, APType::apt_string));
	do_message_params.push_back(ActionParam("password", true, APType::apt_string));
	do_message_params.push_back(ActionParam("realm", false, APType::apt_string));
	do_message_params.push_back(ActionParam("label", true, APType::apt_string));
	do_message_params.push_back(ActionParam("expected_cause_code", false, APType::apt_integer));
	// do_accept_message
	do_accept_message_params.push_back(ActionParam("account", false, APType::apt_string));
	do_accept_message_params.push_back(ActionParam("transport", false, APType::apt_string));
	do_accept_message_params.push_back(ActionParam("label", false, APType::apt_string));
	do_accept_message_params.push_back(ActionParam("message_count", false, APType::apt_integer));
}

void setTurnConfig(AccountConfig &acc_cfg, Config *cfg) {
	turn_config_t *turn_config = &cfg->turn_config;
	LOG(logINFO) << __FUNCTION__ << " enabled:"<<turn_config->enabled<<"["<<turn_config->username<<":"<<turn_config->password<<"]hashed:"<<turn_config->password_hashed;
	if (turn_config->enabled) {
		acc_cfg.natConfig.turnEnabled = true;
		acc_cfg.natConfig.turnServer = turn_config->server;
		acc_cfg.natConfig.turnConnType = PJ_TURN_TP_UDP;
		acc_cfg.natConfig.turnUserName = turn_config->username;
		if (turn_config->password_hashed) {
			acc_cfg.natConfig.turnPasswordType = PJ_STUN_PASSWD_HASHED;
		} else {
			acc_cfg.natConfig.turnPasswordType = PJ_STUN_PASSWD_PLAIN;
		}
		acc_cfg.natConfig.turnPassword = turn_config->password;
		acc_cfg.natConfig.iceEnabled = true;
		if (turn_config->sip_stun_use) {
			acc_cfg.natConfig.sipStunUse = PJSUA_STUN_USE_DEFAULT;
		}
		if (turn_config->media_stun_use) {
			acc_cfg.natConfig.mediaStunUse = PJSUA_STUN_USE_DEFAULT;
		}
	} else if (turn_config->stun_only) {
		if (!turn_config->sip_stun_use && turn_config->media_stun_use) {
			LOG(logINFO) <<__FUNCTION__<<" STUN: enabled without SIP or Media";
		}

		if (turn_config->sip_stun_use) {
			acc_cfg.natConfig.sipStunUse = PJSUA_STUN_USE_DEFAULT;
		} else {
			acc_cfg.natConfig.sipStunUse = PJSUA_STUN_USE_DISABLED;
		}
		if (turn_config->media_stun_use) {
			acc_cfg.natConfig.mediaStunUse = PJSUA_STUN_USE_DEFAULT;
		} else {
			acc_cfg.natConfig.mediaStunUse = PJSUA_STUN_USE_DISABLED;
		}
		acc_cfg.natConfig.sdpNatRewriteUse = false;
		acc_cfg.natConfig.turnEnabled = false;
		acc_cfg.natConfig.turnServer = turn_config->server;
		acc_cfg.natConfig.turnConnType = PJ_TURN_TP_UDP;
		acc_cfg.natConfig.turnUserName = turn_config->username;
		if (turn_config->password_hashed) {
			acc_cfg.natConfig.turnPasswordType = PJ_STUN_PASSWD_HASHED;
		} else {
			acc_cfg.natConfig.turnPasswordType = PJ_STUN_PASSWD_PLAIN;
		}
		acc_cfg.natConfig.turnPassword = turn_config->password;
		acc_cfg.natConfig.iceEnabled = false;
	} else {
		acc_cfg.natConfig.turnEnabled = false;
		acc_cfg.natConfig.iceEnabled = false;
	}

// ret.ice_cfg_use = PJSUA_ICE_CONFIG_USE_CUSTOM;
// ret.ice_cfg.enable_ice = natConfig.iceEnabled;
// ret.ice_cfg.ice_max_host_cands = natConfig.iceMaxHostCands;
// ret.ice_cfg.ice_opt.aggressive = natConfig.iceAggressiveNomination;
// ret.ice_cfg.ice_opt.nominated_check_delay = natConfig.iceNominatedCheckDelayMsec;
// ret.ice_cfg.ice_opt.controlled_agent_want_nom_timeout = natConfig.iceWaitNominationTimeoutMsec;
// ret.ice_cfg.ice_no_rtcp = natConfig.iceNoRtcp;
// ret.ice_cfg.ice_always_update = natConfig.iceAlwaysUpdate;
}

// void setTurnConfigSRTP(AccountConfig &acc_cfg, Config *cfg) {
// 	turn_config_t *turn_config = &cfg->turn_config;
// 	if (turn_config->enabled && !turn_config->stun_only) {
// 		LOG(logINFO) << __FUNCTION__ << " Adjusting TURN config for SRTP";
// 		acc_cfg.natConfig.iceEnabled = true;
// 		acc_cfg.natConfig.iceTrickle = PJ_ICE_SESS_TRICKLE_FULL;
// 		acc_cfg.natConfig.iceAggressiveNomination = true;
// 	}
// }

void Action::do_register(const vector<ActionParam> &params, const vector<ActionCheck> &checks, const SipHeaderVector &x_headers) {
	string type {"register"};
	string transport {"udp"};
	string label {};
	string registrar {};
	string proxy {};
	string realm {"*"};
	string username {};
	string auth_username {};
	string account_name {};
	string account_full_name {};
	string account_aor {};
	string password {};
	string reg_id {};
	string instance_id {};
	string srtp {};
	int expected_cause_code {200};
	bool unregister {false};
	bool rewrite_contact {false};

	for (auto param : params) {
		if (param.name.compare("transport") == 0) transport = param.s_val;
		else if (param.name.compare("label") == 0) label = param.s_val;
		else if (param.name.compare("registrar") == 0) registrar = param.s_val;
		else if (param.name.compare("proxy") == 0) proxy = param.s_val;
		else if (param.name.compare("realm") == 0 && param.s_val != "") realm = param.s_val;
		else if (param.name.compare("account") == 0) account_name = param.s_val;
		else if (param.name.compare("aor") == 0) account_aor = param.s_val;
		else if (param.name.compare("username") == 0) username = param.s_val;
		else if (param.name.compare("auth_username") == 0) auth_username = param.s_val;
		else if (param.name.compare("password") == 0) password = param.s_val;
		else if (param.name.compare("reg_id") == 0) reg_id = param.s_val;
		else if (param.name.compare("instance_id") == 0) instance_id = param.s_val;
		else if (param.name.compare("unregister") == 0) unregister = param.b_val;
		else if (param.name.compare("rewrite_contact") == 0) rewrite_contact = param.b_val;
		else if (param.name.compare("expected_cause_code") == 0) expected_cause_code = param.i_val;
		else if (param.name.compare("srtp") == 0 && param.s_val.length() > 0) srtp = param.s_val;
	}

	if (username.empty() || password.empty() || registrar.empty()) {
		LOG(logERROR) << __FUNCTION__ << " missing action parameter" ;
		return;
	}
	vp::tolower(transport);

	if (account_name.empty()) {
		account_name = username;
	}
	filter_accountname(&account_name);

	if (auth_username.empty()) {
		auth_username = username;
	}
	if (account_aor.empty()) {
		account_aor = username + "@" + registrar;
	}
	// This should be just internal identifier for program
	account_full_name = account_name + "@" + registrar;

	TestAccount *acc = config->findAccount(account_full_name);

	if (unregister) {
		if (acc) {
			// We should probably create a new test ...
			if (acc->test) acc->test->type = "unregister";
			LOG(logINFO) << __FUNCTION__ << " unregister (" << account_full_name << ")";
			AccountInfo acc_inf = acc->getInfo();
			if (acc_inf.regIsActive) {
				LOG(logINFO) << __FUNCTION__ << " register is active";
				try {
					acc->setRegistration(false);
					acc->unregistering = true;
				} catch (pj::Error& e)  {
					LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
				}
			} else {
				LOG(logINFO) << __FUNCTION__ << " register is not active";
			}
			int max_wait_ms = 2000;
			while (acc->unregistering && max_wait_ms >= 0) {
				pj_thread_sleep(10);
				max_wait_ms -= 10;
				// acc_inf = acc->getInfo();
			}
			if (acc->unregistering) {
				LOG(logERROR) << __FUNCTION__ << " error : unregister failed/timeout"<< std::endl;
			}
			return;
		}
		LOG(logINFO) << __FUNCTION__ << "unregister: account not found (" << account_full_name << ")";
	}

	Test *test = new Test(config, type);
	test->local_user = username;
	test->remote_user = username;
	test->label = label;
	test->expected_cause_code = expected_cause_code;
	test->from = username;
	test->type = type;
	test->srtp = srtp;

	LOG(logINFO) << __FUNCTION__ << " >> sip:" + account_full_name;
	AccountConfig acc_cfg;
	SipHeader sh;
	sh.hName = "User-Agent";
	sh.hValue = "<volts_voip_patrol>";
	acc_cfg.regConfig.headers.push_back(sh);
	setTurnConfig(acc_cfg, config);

	if (reg_id != "" || instance_id != "") {
		LOG(logINFO) << __FUNCTION__ << " reg_id:" << reg_id << " instance_id:" << instance_id;
		if (transport == "udp") {
			LOG(logINFO) << __FUNCTION__ << " oubound rfc5626 not supported on transport UDP";
		} else {
			acc_cfg.natConfig.sipOutboundUse = true;
			if (reg_id != "")
				acc_cfg.natConfig.sipOutboundRegId = reg_id;
			if (instance_id != "")
				acc_cfg.natConfig.sipOutboundInstanceId = instance_id;
		}
	} else {
		acc_cfg.natConfig.sipOutboundUse = false;
	}
	for (auto x_hdr : x_headers) {
		acc_cfg.regConfig.headers.push_back(x_hdr);
	}

	if (transport == "tcp") {
		LOG(logINFO) << __FUNCTION__ << " SIP TCP";
		acc_cfg.idUri = "sip:" + account_aor + ";transport=tcp";
		acc_cfg.regConfig.registrarUri = "sip:" + registrar + ";transport=tcp";
		if (!proxy.empty()) {
			acc_cfg.sipConfig.proxies.push_back("sip:" + proxy + ";transport=tcp");
		}
	} else if (transport == "tls") {
		if (config->transport_id_tls == -1) {
			LOG(logERROR) << __FUNCTION__ << " TLS transport not supported";
			return;
		}
		acc_cfg.idUri = "sip:" + account_aor + ";transport=tls";
		acc_cfg.regConfig.registrarUri = "sip:" + registrar + ";transport=tls";
		if (!proxy.empty()) {
			acc_cfg.sipConfig.proxies.push_back("sip:" + proxy + ";transport=tls");
		}
	} else if (transport == "sips") {
		if (config->transport_id_tls == -1) {
			LOG(logERROR) << __FUNCTION__ << " TLS transport not supported";
			return;
		}
		acc_cfg.idUri = "sips:" + account_aor;
		acc_cfg.regConfig.registrarUri = "sips:" + registrar;
		if (!proxy.empty()) {
			acc_cfg.sipConfig.proxies.push_back("sips:" + proxy);
		}
		LOG(logINFO) << __FUNCTION__ << " SIPS/TLS URI Scheme";
	} else {
		LOG(logINFO) << __FUNCTION__ << " SIP UDP";
		acc_cfg.idUri = "sip:" + account_aor;
		acc_cfg.regConfig.registrarUri = "sip:" + registrar;
		if (!proxy.empty())
			acc_cfg.sipConfig.proxies.push_back("sip:" + proxy);
	}
	acc_cfg.sipConfig.authCreds.push_back(AuthCredInfo("digest", realm, auth_username, 0, password));
	acc_cfg.natConfig.contactRewriteUse = rewrite_contact;
	acc_cfg.sipConfig.contactUriParams = ";vp_acc=" + account_name;

	// SRTP for incoming calls
	if (srtp.find("dtls") != std::string::npos) {
		acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
		acc_cfg.mediaConfig.srtpOpt.keyings.push_back(PJMEDIA_SRTP_KEYING_DTLS_SRTP);
		LOG(logINFO) << __FUNCTION__ << " adding DTLS-SRTP capabilities";
	}
	if (srtp.find("sdes") != std::string::npos) {
		acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
		acc_cfg.mediaConfig.srtpOpt.keyings.push_back(PJMEDIA_SRTP_KEYING_SDES);
		LOG(logINFO) << __FUNCTION__ << " adding SDES capabilities";
	}
	if (srtp.find("force") != std::string::npos) {
		acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_MANDATORY;
		LOG(logINFO) << __FUNCTION__ << " Forcing SRTP";
	}

	if (!acc) {
		acc = config->createAccount(acc_cfg);
	} else {
		acc->modify(acc_cfg);
	}
	acc->setTest(test);
	acc->account_name = account_name;
}

void Action::do_accept(const vector<ActionParam> &params, const vector<ActionCheck> &checks, const pj::SipHeaderVector &x_headers) {
	string type {"accept"};
	string account_name {};
	string transport {};
	string label {};
	string play {default_playback_file};
	string recording {};
	string play_dtmf {};
	string timer {};
	string cancel_behavoir {};
	//float min_mos {0.0};
	int max_duration {0};
	int ring_duration {0};
	int early_media {false};
	int hangup_duration {0};
	int re_invite_interval {0};
	call_state_t wait_until {INV_STATE_NULL};
	bool rtp_stats {false};
	bool late_start {false};
	bool fail_on_accept {false};
	string srtp {"none"};
	string force_contact {};
	int code {200};
	int expected_cause_code {200};
	int call_count {-1};
	int response_delay {0};
	string reason {};

	for (auto param : params) {
		if (param.name.compare("match_account") == 0) account_name = param.s_val;
		else if (param.name.compare("transport") == 0) transport = param.s_val;
		else if (param.name.compare("play") == 0 && param.s_val.length() > 0) play = param.s_val;
		else if (param.name.compare("record") == 0) recording = param.s_val;
		else if (param.name.compare("play_dtmf") == 0 && param.s_val.length() > 0) play_dtmf = param.s_val;
		else if (param.name.compare("timer") == 0 && param.s_val.length() > 0) timer = param.s_val;
		else if (param.name.compare("code") == 0) code = param.i_val;
		else if (param.name.compare("expected_cause_code") == 0) expected_cause_code = param.i_val;
		else if (param.name.compare("call_count") == 0) call_count = param.i_val;
		else if (param.name.compare("reason") == 0 && param.s_val.length() > 0) reason = param.s_val;
		else if (param.name.compare("label") == 0 && param.s_val.length() > 0) label = param.s_val;
		else if (param.name.compare("max_duration") == 0) max_duration = param.i_val;
		else if (param.name.compare("ring_duration") == 0) ring_duration = param.i_val;
		else if (param.name.compare("early_media") == 0) early_media = param.b_val;
		else if (param.name.compare("fail_on_accept") == 0) fail_on_accept = param.b_val;
		//else if (param.name.compare("min_mos") == 0) min_mos = param.f_val;
		else if (param.name.compare("rtp_stats") == 0) rtp_stats = param.b_val;
		else if (param.name.compare("srtp") == 0 && param.s_val.length() > 0) srtp = param.s_val;
		else if (param.name.compare("force_contact") == 0) force_contact = param.s_val;
		else if (param.name.compare("late_start") == 0) late_start = param.b_val;
		else if (param.name.compare("wait_until") == 0) wait_until = get_call_state_from_string(param.s_val);
		else if (param.name.compare("hangup") == 0) hangup_duration = param.i_val;
		else if (param.name.compare("cancel") == 0) cancel_behavoir = param.s_val;
		else if (param.name.compare("re_invite_interval") == 0) re_invite_interval = param.i_val;
		else if (param.name.compare("response_delay") == 0) response_delay = param.i_val;
	}

	if (account_name.empty()) {
		LOG(logERROR) << __FUNCTION__ << " missing action parameters <match_account>" ;
		config->total_tasks_count += 100;

		return;
	}
	filter_accountname(&account_name);

	vp::tolower(transport);

	TestAccount *acc = config->findAccount(account_name);
	if (!acc || !force_contact.empty()) {
		AccountConfig acc_cfg;
		setTurnConfig(acc_cfg, config);

		if (!force_contact.empty()){
			LOG(logINFO) << __FUNCTION__ << ":do_accept:force_contact:" << force_contact << "\n";
			acc_cfg.sipConfig.contactForced = force_contact;
		}

		if (!transport.empty()) {
			if (transport == "tcp") {
				acc_cfg.sipConfig.transportId = config->transport_id_tcp;
			} else if (transport == "udp") {
				acc_cfg.sipConfig.transportId = config->transport_id_udp;
			} else if (transport == "tls" || transport == "sips") {
				if (config->transport_id_tls == -1) {
					LOG(logERROR) << __FUNCTION__ << ": TLS transport not supported.";
					return;
				}
				acc_cfg.sipConfig.transportId = config->transport_id_tls;
			}
		}
		if (acc_cfg.sipConfig.transportId == config->transport_id_tls && transport == "sips") {
			acc_cfg.idUri = "sips:" + account_name;
		} else {
			acc_cfg.idUri = "sip:" + account_name;
		}
		if (!timer.empty()) {
			if (timer.compare("inactive") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_INACTIVE;
			} else if (timer.compare("optionnal") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_OPTIONAL;
			} else if (timer.compare("required") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_REQUIRED;
			} else if (timer.compare("always") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_ALWAYS;
			}
			LOG(logINFO) << __FUNCTION__ << ":session timer["<<timer<<"]: "<< acc_cfg.callConfig.timerUse ;
		}

		// SRTP
		if (srtp.find("dtls") != std::string::npos) {
			acc_cfg.mediaConfig.srtpOpt.keyings.push_back(PJMEDIA_SRTP_KEYING_DTLS_SRTP);
			acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
			LOG(logINFO) << __FUNCTION__ << " adding DTLS-SRTP capabilities";
		}
		if (srtp.find("sdes") != std::string::npos) {
			acc_cfg.mediaConfig.srtpOpt.keyings.push_back(PJMEDIA_SRTP_KEYING_SDES);
			acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
			LOG(logINFO) << __FUNCTION__ << " adding SDES capabilities";
		}
		if (srtp.find("force") != std::string::npos) {
			acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_MANDATORY;
			LOG(logINFO) << __FUNCTION__ << " Forcing SRTP";
		}

		if (acc) {
			acc->modify(acc_cfg);
		} else {
			acc = config->createAccount(acc_cfg);
		}
	}

	if (fail_on_accept) {
		config->total_tasks_count -= 1;
		LOG(logINFO) << __FUNCTION__ << " decreasing task counter to " << config->total_tasks_count << " due to this accept should not happen";
	}

	if (expected_cause_code < 100 || expected_cause_code > 700) {
		expected_cause_code = 200;
	}

	if (code < 100 || code > 700) {
		code = 200;
	}

	acc->hangup_duration = hangup_duration;
	acc->re_invite_interval = re_invite_interval;
	acc->response_delay = response_delay;
	acc->max_duration = max_duration;
	acc->ring_duration = ring_duration;
	acc->accept_label = label;
	acc->rtp_stats = rtp_stats;
	acc->late_start = late_start;
	acc->play = play;
	acc->recording = recording;
	acc->play_dtmf = play_dtmf;
	acc->timer = timer;
	acc->early_media = early_media;
	acc->wait_state = wait_until;
	acc->reason = reason;
	acc->code = code;
	acc->expected_cause_code = expected_cause_code;
	acc->call_count = call_count;
	acc->x_headers = x_headers;
	acc->checks = checks;
	acc->srtp = srtp;
	acc->force_contact = force_contact;
	acc->cancel_behavoir = cancel_behavoir;
	acc->fail_on_accept	= fail_on_accept;
	acc->account_name = account_name;
}


void Action::do_call(const vector<ActionParam> &params, const vector<ActionCheck> &checks, const SipHeaderVector &x_headers) {
	string type {"call"};
	string play {default_playback_file};
	string play_dtmf {};
	string timer {};
	string caller {};
	string from {};
	string callee {};
	string to_uri {};
	string transport {"udp"};
	string username {};
	string auth_username {};
	string password {};
	string realm {"*"};
	string label {};
	string proxy {};
	string srtp {"none"};
	int expected_cause_code {200};
	call_state_t wait_until {INV_STATE_NULL};
	float min_mos {0.0};
	int max_duration {0};
	int max_ring_duration {0};
	int expected_duration {0};
	int hangup_duration {0};
	int re_invite_interval {0};
	int repeat {0};
	string recording {};
	bool rtp_stats {false};
	bool late_start {false};
	string force_contact {};

	for (auto param : params) {
		if (param.name.compare("callee") == 0) callee = param.s_val;
		else if (param.name.compare("caller") == 0) caller = param.s_val;
		else if (param.name.compare("from") == 0) from = param.s_val;
		else if (param.name.compare("to_uri") == 0) to_uri = param.s_val;
		else if (param.name.compare("transport") == 0) transport = param.s_val;
		else if (param.name.compare("play") == 0 && param.s_val.length() > 0) play = param.s_val;
		else if (param.name.compare("record") == 0) recording = param.s_val;
		else if (param.name.compare("play_dtmf") == 0 && param.s_val.length() > 0) play_dtmf = param.s_val;
		else if (param.name.compare("timer") == 0 && param.s_val.length() > 0) timer = param.s_val;
		else if (param.name.compare("username") == 0) username = param.s_val;
		else if (param.name.compare("auth_username") == 0) auth_username = param.s_val;
		else if (param.name.compare("password") == 0) password = param.s_val;
		else if (param.name.compare("realm") == 0 && param.s_val != "") realm = param.s_val;
		else if (param.name.compare("label") == 0) label = param.s_val;
		else if (param.name.compare("proxy") == 0) proxy = param.s_val;
		else if (param.name.compare("expected_cause_code") == 0) expected_cause_code = param.i_val;
		else if (param.name.compare("wait_until") == 0) wait_until = get_call_state_from_string(param.s_val);
		else if (param.name.compare("min_mos") == 0) min_mos = param.f_val;
		else if (param.name.compare("rtp_stats") == 0) rtp_stats = param.b_val;
		else if (param.name.compare("late_start") == 0) late_start = param.b_val;
		else if (param.name.compare("srtp") == 0 && param.s_val.length() > 0) srtp = param.s_val;
		else if (param.name.compare("force_contact") == 0) force_contact = param.s_val;
		else if (param.name.compare("max_duration") == 0) max_duration = param.i_val;
		else if (param.name.compare("max_ring_duration") == 0) max_ring_duration = param.i_val;
		else if (param.name.compare("duration") == 0) expected_duration = param.i_val;
		else if (param.name.compare("hangup") == 0) hangup_duration = param.i_val;
		else if (param.name.compare("re_invite_interval") == 0) re_invite_interval = param.i_val;
		else if (param.name.compare("repeat") == 0) repeat = param.i_val;
	}

	if (caller.empty() || callee.empty()) {
		LOG(logERROR) << __FUNCTION__ << ": missing action parameters <callee>/<caller>" ;

		config->total_tasks_count += 100;
		return;
	}
	vp::tolower(transport);

	string account_uri {caller};
	if (transport != "udp") {
		account_uri = caller + ";transport=" + transport;
	}
	TestAccount* acc = config->findAccount(account_uri);
	if (!acc) {
		AccountConfig acc_cfg;
		LOG(logINFO) << __FUNCTION__ << ":do_call:turn:" << config->turn_config.enabled << "\n";
		setTurnConfig(acc_cfg, config);

		if (force_contact != ""){
			LOG(logINFO) << __FUNCTION__ << ":do_call:force_contact:" << force_contact << "\n";
			acc_cfg.sipConfig.contactForced = force_contact;
		}

		if (!timer.empty()) {
			if (timer.compare("inactive") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_INACTIVE;
			} else if (timer.compare("optionnal") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_OPTIONAL;
			} else if (timer.compare("required") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_REQUIRED;
			} else if (timer.compare("always") == 0) {
				acc_cfg.callConfig.timerUse = PJSUA_SIP_TIMER_ALWAYS;
			}
			LOG(logINFO) << __FUNCTION__ << ": session timer[" << timer << "] : " << acc_cfg.callConfig.timerUse ;
		}

		if (transport == "tcp") {
			acc_cfg.idUri = "sip:" + account_uri;
			if (!proxy.empty())
				acc_cfg.sipConfig.proxies.push_back("sip:" + proxy + ";transport=tcp");
		} else if (transport == "tls") {
			if (config->transport_id_tls == -1) {
				LOG(logERROR) << __FUNCTION__ << ": TLS transport not supported" ;
				return;
			}
			acc_cfg.idUri = "tls:" + account_uri;
			if (!proxy.empty())
				acc_cfg.sipConfig.proxies.push_back("sip:" + proxy + ";transport=tls");
		} else if (transport == "sips") {
			if (config->transport_id_tls == -1) {
				LOG(logERROR) << __FUNCTION__ << ": sips(TLS) transport not supported" ;
				return;
			}
			acc_cfg.idUri = "sips:" + account_uri;
			if (!proxy.empty())
				acc_cfg.sipConfig.proxies.push_back("sips:" + proxy);
		} else {
			acc_cfg.idUri = "sip:" + account_uri;
			if (!proxy.empty())
				acc_cfg.sipConfig.proxies.push_back("sip:" + proxy);
		}

		if (!from.empty()) {
			acc_cfg.idUri = from;
		}

		if (!username.empty() || !auth_username.empty()) {
			if (password.empty()) {
				LOG(logERROR) << __FUNCTION__ << ": realm specified missing password";
				return;
			}
			if (auth_username.empty()) {
				auth_username = username;
			}
			acc_cfg.sipConfig.authCreds.push_back( AuthCredInfo("digest", realm, auth_username, 0, password) );
		}

		// SRTP
		if (srtp.find("dtls") != std::string::npos) {
			acc_cfg.mediaConfig.srtpOpt.keyings.push_back(PJMEDIA_SRTP_KEYING_DTLS_SRTP);
			acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
			LOG(logINFO) << __FUNCTION__ << " adding DTLS-SRTP capabilities";
		}
		if (srtp.find("sdes") != std::string::npos) {
			acc_cfg.mediaConfig.srtpOpt.keyings.push_back(PJMEDIA_SRTP_KEYING_SDES);
			acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_OPTIONAL;
			LOG(logINFO) << __FUNCTION__ << " adding SDES capabilities";
		}
		if (srtp.find("force") != std::string::npos) {
			acc_cfg.mediaConfig.srtpUse = PJMEDIA_SRTP_MANDATORY;
			LOG(logINFO) << __FUNCTION__ << " Forcing SRTP";
		}

		// FIXME
		// if (acc_cfg.mediaConfig.srtpUse == PJMEDIA_SRTP_OPTIONAL || acc_cfg.mediaConfig.srtpUse == PJMEDIA_SRTP_MANDATORY) {
		// 	setTurnConfigSRTP(acc_cfg, config);
		// }

		acc = config->createAccount(acc_cfg);
		LOG(logINFO) << __FUNCTION__ << ": session timer["<<timer<<"] :"<< acc_cfg.callConfig.timerUse << " TURN: "<< acc_cfg.natConfig.turnEnabled;
	}

	do {
		Test *test = new Test(config, type);
		test->wait_state = wait_until;
		if (test->wait_state != INV_STATE_NULL)
			test->state = VPT_RUN_WAIT;
		test->expected_duration = expected_duration;
		test->label = label;
		test->play = play;
		test->play_dtmf = play_dtmf;
		test->min_mos = min_mos;
		test->max_duration = max_duration;
		test->max_ring_duration = max_ring_duration;
		test->hangup_duration = hangup_duration;
		test->re_invite_interval = re_invite_interval;
		test->re_invite_next = re_invite_interval;
		test->recording = recording;
		test->rtp_stats = rtp_stats;
		test->late_start = late_start;
		test->force_contact = force_contact;
		test->srtp = srtp;
		std::size_t pos = caller.find("@");
		if (pos!=std::string::npos) {
			test->local_user = caller.substr(0, pos);
		}
		pos = callee.find("@");
		if (pos!=std::string::npos) {
			test->remote_user = callee.substr(0, pos);
		}

		TestCall *call = new TestCall(acc);
		config->calls.push_back(call);

		call->test = test;
		test->expected_cause_code = expected_cause_code;
		test->from = caller;
		test->to = callee;
		test->type = type;
		acc->calls.push_back(call);
		CallOpParam prm(true);

		for (auto x_hdr : x_headers) {
			prm.txOption.headers.push_back(x_hdr);
		}

		prm.opt.audioCount = 1;
		prm.opt.videoCount = 0;
		LOG(logINFO) << "call->test:" << test << " " << call->test->type;
		LOG(logINFO) << "calling :" +callee;
		if (transport == "tls") {
			if (!to_uri.empty())
					to_uri = "sip:"+to_uri+";transport=tls";
			try {
				call->makeCall("sip:"+callee+";transport=tls", prm, to_uri);
			} catch (pj::Error& e)  {
				LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
			}
		} else if (transport == "sips") {
			if (!to_uri.empty())
					to_uri = "sips:"+to_uri;
			try {
				call->makeCall("sips:"+callee, prm, to_uri);
			} catch (pj::Error& e)  {
				LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
			}
		} else if (transport == "tcp") {
			if (!to_uri.empty())
				to_uri = "sip:"+to_uri+";transport=tcp";
			try {
				call->makeCall("sip:"+callee+";transport=tcp", prm, to_uri);
			} catch (pj::Error& e)  {
				LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
			}
		} else {
			if (!to_uri.empty())
					to_uri = "sip:"+to_uri;
			try {
				call->makeCall("sip:"+callee, prm, to_uri);
			} catch (pj::Error& e)  {
				LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
			}
		}
		repeat--;
	} while (repeat >= 0);
}

void Action::do_turn(const vector<ActionParam> &params) {
	bool enabled {false};
	string server {};
	string username {};
	string password {};
	bool password_hashed {false};
	bool stun_only {false};
	bool sip_stun_use {false};
	bool media_stun_use {false};
	for (auto param : params) {
		if (param.name.compare("enabled") == 0) enabled = param.b_val;
		else if (param.name.compare("server") == 0) server = param.s_val;
		else if (param.name.compare("username") == 0) username = param.s_val;
		else if (param.name.compare("password") == 0) password = param.s_val;
		else if (param.name.compare("password_hashed") == 0) password_hashed = param.b_val;
		else if (param.name.compare("sip_stun_use") == 0) sip_stun_use = param.b_val;
		else if (param.name.compare("media_stun_use") == 0) media_stun_use = param.b_val;
		else if (param.name.compare("stun_only") == 0) stun_only = param.b_val;
	}
	LOG(logINFO) << __FUNCTION__ << " enabled["<<enabled<<"] server["<<server<<"] username["<<username<<"] password["<<password<<"]:"<<password_hashed;
	config->turn_config.enabled = enabled;
	config->turn_config.server = server;
	config->turn_config.password_hashed = password_hashed;
	if (!username.empty())
		config->turn_config.username = username;
	if (!password.empty())
		config->turn_config.password = password;
	config->turn_config.media_stun_use = media_stun_use;
	config->turn_config.sip_stun_use = sip_stun_use;
	config->turn_config.stun_only = stun_only;
}


void Action::do_message(const vector<ActionParam> &params, const vector<ActionCheck> &checks, const SipHeaderVector &x_headers) {
	string to_uri {};
	string from {};
	string text {};
	string transport {"udp"};
	string username {};
	string password {};
	string realm {"*"};
	string label {};
	int expected_cause_code {200};
	for (auto param : params) {
		if (param.name.compare("from") == 0) from = param.s_val;
		else if (param.name.compare("to_uri") == 0) to_uri = param.s_val;
		else if (param.name.compare("text") == 0) text = param.s_val;
		else if (param.name.compare("transport") == 0) transport = param.s_val;
		else if (param.name.compare("username") == 0) username = param.s_val;
		else if (param.name.compare("password") == 0) password = param.s_val;
		else if (param.name.compare("realm") == 0 && param.s_val != "") realm = param.s_val;
		else if (param.name.compare("label") == 0) label = param.s_val;
		else if (param.name.compare("expected_cause_code") == 0) expected_cause_code = param.i_val;
	}

    string buddy_uri = "<sip:" + to_uri + ">";
    BuddyConfig bCfg;
    bCfg.uri = buddy_uri;
	bCfg.subscribe = false;

	TestAccount *acc = config->findAccount(from);
	string account_uri = from;
	vp::tolower(transport);
	if (transport != "udp") {
		 account_uri = "sip:" + account_uri + ";transport=" + transport;
	} else {
		 account_uri = "sip:" + account_uri;
	}
	if (!acc) { // account not found, creating one
		AccountConfig acc_cfg;
		acc_cfg.idUri = account_uri;
		acc_cfg.sipConfig.authCreds.push_back(AuthCredInfo("digest", realm, username, 0, password));

		LOG(logINFO) <<__FUNCTION__ << ": create buddy account_uri:" << account_uri << "\n";

		acc = config->createAccount(acc_cfg);
	}

	Buddy buddy;
	Account& account = *acc;
    buddy.create(account, bCfg);
    // buddy.delete();
	string type{"message"};

	Test *test = new Test(config, type);
	test->local_user = username;
	test->remote_user = username;
	test->label = label;
	test->expected_cause_code = expected_cause_code;
	test->from = username;
	test->type = type;
	acc->test = test;

	SendInstantMessageParam param;
	param.content = text;
	param.txOption.targetUri = buddy_uri;

	LOG(logINFO) <<__FUNCTION__ << "sending... InstantMessage\n";

	buddy.sendInstantMessage(param);

	LOG(logINFO) <<__FUNCTION__ << ": sent InstantMessage\n";
}

void Action::do_accept_message(const vector<ActionParam> &params, const vector<ActionCheck> &checks, const pj::SipHeaderVector &x_headers) {
	string type {"accept_message"};
	string account_name {};
	string transport {};
	int code {200};
	int message_count {1};
	string label {};
	string reason {};
	string expected_message {};
	for (auto param : params) {
		if (param.name.compare("account") == 0) account_name = param.s_val;
		else if (param.name.compare("transport") == 0) transport = param.s_val;
		else if (param.name.compare("code") == 0) code = param.i_val;
		else if (param.name.compare("message_count") == 0) message_count = param.i_val;
		else if (param.name.compare("reason") == 0 && param.s_val.length() > 0) reason = param.s_val;
		else if (param.name.compare("label") == 0) label = param.s_val;
		else if (param.name.compare("expected_message") == 0) expected_message = param.s_val;
	}

	if (account_name.empty()) {
		LOG(logERROR) <<__FUNCTION__<< " missing action parameters <account>";

		config->total_tasks_count += 100;
		return;
	}
	vp::tolower(transport);

	TestAccount *acc = config->findAccount(account_name);
	AccountConfig acc_cfg;
	if (!acc) {
		if (!transport.empty()) {
			if (transport == "tcp") {
				acc_cfg.sipConfig.transportId = config->transport_id_tcp;
			} else if (transport == "udp") {
				acc_cfg.sipConfig.transportId = config->transport_id_udp;
			} else if (transport == "tls" || transport == "sips") {
				if (config->transport_id_tls == -1) {
					LOG(logERROR) <<__FUNCTION__<<": TLS transport not supported.";

					return;
				}
				acc_cfg.sipConfig.transportId = config->transport_id_tls;
			}
		}
		if (acc_cfg.sipConfig.transportId == config->transport_id_tls && transport == "sips") {
			acc_cfg.idUri = "sips:" + account_name;
		} else {
			acc_cfg.idUri = "sip:" + account_name;
		}

		if (acc) {
			acc->modify(acc_cfg);
		} else {
			acc = config->createAccount(acc_cfg);
		}
	}
	acc->accept_label = label;
	acc->reason = reason;
	acc->code = code;
	acc->message_count = message_count;
	acc->x_headers = x_headers;
	acc->checks = checks;

	Test *test = new Test(config, type);
	test->checks = checks;
	test->expected_cause_code = 200;
	acc->testAccept = test;
}

void Action::do_codec(const vector<ActionParam> &params) {
	string enable {};
	int priority {0};
	string disable {};
	for (auto param : params) {
		if (param.name.compare("enable") == 0) enable = param.s_val;
		else if (param.name.compare("priority") == 0) priority = param.i_val;
		else if (param.name.compare("disable") == 0) disable = param.s_val;
	}
	LOG(logINFO) << __FUNCTION__ << " enable["<<enable<<"] with priority["<<priority<<"] disable["<<disable<<"]";
	if (!config->ep) {
		LOG(logERROR) << __FUNCTION__ << " PJSIP endpoint not available";
		return;
	}
	if (!disable.empty())
		config->ep->setCodecs(disable, 0);
	if (!enable.empty())
		config->ep->setCodecs(enable, priority);
}

void Action::do_alert(const vector<ActionParam> &params) {
	string email {};
	string email_from {};
	string smtp_host {};
	for (auto param : params) {
		if (param.name.compare("email") == 0) email = param.s_val;
		else if (param.name.compare("email_from") == 0) email_from = param.s_val;
		else if (param.name.compare("smtp_host") == 0) smtp_host = param.s_val;
	}
	LOG(logINFO) << __FUNCTION__ << "email to:"<<email<< " from:"<<email_from;
	config->alert_email_to = email;
	config->alert_email_from = email_from;
	config->alert_server_url = smtp_host;
}

void Action::do_wait(const vector<ActionParam> &params) {
	int duration_ms = 0;
	bool complete_all = false;
	for (auto param : params) {
		if (param.name.compare("ms") == 0) {
			duration_ms = param.i_val;
		}
		if (param.name.compare("complete") == 0) {
			complete_all = param.b_val;
		}
	}
	LOG(logINFO) << __FUNCTION__ << " processing duration_ms:" << duration_ms << " complete all tests:" << complete_all;
	bool completed = false;
	int tests_running = 0;
	bool status_update = true;
	while (!completed) {
		for (auto & account : config->accounts) {
			AccountInfo acc_inf = account->getInfo();

			if (account->test && account->test->state == VPT_DONE) {
				delete account->test;
				account->test = NULL;
			} else if (account->test) {
				tests_running += 1;
			}
			// accept/call_count, are considered "tests_running" when maximum duration is either not specified or reached.
			if (account->call_count > 0 && (duration_ms > 0 || duration_ms == -1)) {
				tests_running += 1;
			}
			// accept/message_count, are considered "tests_running" when maximum duration is either not specified or reached.
			if (account->message_count > 0 && (duration_ms > 0 || duration_ms == -1)) {
				tests_running++;
			}
		}

		// prevent calls destruction while parsing looking at them
		config->checking_calls.lock();

		for (auto & call : config->calls) {
			if (call->test && call->test->state == VPT_DONE){
				//CallInfo ci = call->getInfo();
				//if (ci.state == PJSIP_INV_STATE_DISCONNECTED)
				//LOG(logINFO) << "delete call test["<<call->test<<"] = " << config->removeCall(call);
				continue;
			} else if (call->test) {
				CallInfo ci = call->getInfo();
				if (status_update) {
					LOG(logDEBUG) << __FUNCTION__ << ": [call][" << call->getId() << "][test][" << (ci.role==0?"CALLER":"CALLEE") << "]["
						     << ci.callIdString << "][" << ci.remoteUri << "][" << ci.stateText << "|" << ci.state << "]duration["
						     << ci.connectDuration.sec << ">=" << call->test->hangup_duration<< "]";
				}
				if (ci.state == PJSIP_INV_STATE_CALLING || ci.state == PJSIP_INV_STATE_EARLY || ci.state == PJSIP_INV_STATE_INCOMING)  {
					Test *test = call->test;
					if (test->response_delay > 0 && ci.totalDuration.sec >= test->response_delay && ci.state == PJSIP_INV_STATE_INCOMING) {
						CallOpParam prm;
						// Explicitly answer with 100
						CallOpParam prm_100;

						prm_100.statusCode = PJSIP_SC_TRYING;
						call->answer(prm_100);

						if (test->ring_duration > 0) {

							prm.statusCode = PJSIP_SC_RINGING;
							if (test->early_media) {
								prm.statusCode = PJSIP_SC_PROGRESS;
							}

							call->answer(prm);
						} else {
							prm.reason = "OK";
							if (test->code) {
								prm.statusCode = test->code;
							} else {
								prm.statusCode = PJSIP_SC_OK;
							}
							call->answer(prm);
						}
						LOG(logINFO) << " Answering call[" << call->getId() << "] with " << prm.statusCode << " on call time: " << ci.totalDuration.sec;

					} else if (test->ring_duration > 0 && ci.totalDuration.sec >= (test->ring_duration + test->response_delay)) {
						CallOpParam prm;
						prm.reason = "OK";

						if (test->code) {
							prm.statusCode = test->code;
						} else {
							prm.statusCode = PJSIP_SC_OK;
						}

						LOG(logINFO) << " Answering call[" << call->getId() << "] with " << test->code << " on call time: " << ci.totalDuration.sec;

						call->answer(prm);
					} else if (test->max_ring_duration && (test->max_ring_duration + test->response_delay) <= ci.totalDuration.sec) {
						LOG(logINFO) << __FUNCTION__ << "[cancelling:call][" << call->getId() << "][test][" << (ci.role==0?"CALLER":"CALLEE") << "]["
						     << ci.callIdString << "][" << ci.remoteUri << "][" << ci.stateText << "|" << ci.state << "]duration["
						     << ci.totalDuration.sec << ">=(" << test->max_ring_duration << " + " << test->response_delay << ")]";
						CallOpParam prm(true);
						try {
							call->hangup(prm);
						} catch (pj::Error& e)  {
							if (e.status != 171140) {
								LOG(logERROR) << __FUNCTION__ << " error :" << e.status;
							}
						}
					}
				} else if (ci.state == PJSIP_INV_STATE_CONFIRMED) {
					std::string res = "call[" + std::to_string(ci.lastStatusCode) + "] reason[" + ci.lastReason + "]";
					call->test->connect_duration = ci.connectDuration.sec;
					call->test->setup_duration = ci.totalDuration.sec - ci.connectDuration.sec;
					call->test->result_cause_code = (int)ci.lastStatusCode;
					call->test->reason = ci.lastReason;
					// check re-invite
					if (call->test->re_invite_interval && ci.connectDuration.sec >= call->test->re_invite_next){
						if (ci.state == PJSIP_INV_STATE_CONFIRMED) {
							CallOpParam prm(true);
							prm.opt.audioCount = 1;
							prm.opt.videoCount = 0;
							LOG(logINFO) << __FUNCTION__ << " re-invite : call in PJSIP_INV_STATE_CONFIRMED" ;
							try {
								call->reinvite(prm);
								call->test->re_invite_next = call->test->re_invite_next + call->test->re_invite_interval;
							} catch (pj::Error& e)  {
								if (e.status != 171140) {
									LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
								}
							}
						}
					}
					// check hangup
					if (call->test->hangup_duration && ci.connectDuration.sec >= call->test->hangup_duration){
						if (ci.state == PJSIP_INV_STATE_CONFIRMED) {
							CallOpParam prm(true);
							LOG(logINFO) << "hangup : call in PJSIP_INV_STATE_CONFIRMED" ;
							try {
								call->hangup(prm);
							} catch (pj::Error& e)  {
								if (e.status != 171140) {
									LOG(logERROR) << __FUNCTION__ << " error :" << e.status << std::endl;
								}
							}
						}
						call->test->update_result();
					}
				}
				if (complete_all || call->test->state == VPT_RUN_WAIT) {
					tests_running += 1;
				}
			}
		}

		int pos=0;
		for (auto test : config->tests_with_rtp_stats) {
			if (test->rtp_stats_ready) {
				test->update_result();
				config->tests_with_rtp_stats.erase(config->tests_with_rtp_stats.begin()+pos);
				LOG(logINFO) << __FUNCTION__ << " erase pos:" << pos;
			} else {
				tests_running += 1;
				pos += 1;
 			}
		}
		// calls, can now be destroyed
		config->checking_calls.unlock();

		if (tests_running == 0 && complete_all) {
			LOG(logINFO) << __FUNCTION__ << LOG_COLOR_ERROR << ": action[wait] No more tests are running, exiting... " << LOG_COLOR_END;
			completed = true;
		}

		if (duration_ms <= 0 && duration_ms != -1) {
			LOG(logINFO) << __FUNCTION__ << LOG_COLOR_ERROR << ": action[wait] Overall duration exceeded, exiting... " << LOG_COLOR_END;
			completed = true;
		}

		if (tests_running > 0 && complete_all) {
			if (status_update) {
				LOG(logINFO) << __FUNCTION__ <<LOG_COLOR_ERROR<<": action[wait] active account tests or call tests in run_wait["<<tests_running<<"] <<<<"<<LOG_COLOR_END;
				status_update = false;
			}
			tests_running = 0;

			if (duration_ms > 0) {
				duration_ms -= 100;
			}

			pj_thread_sleep(100);
		} else {
			if (status_update) {
				LOG(logINFO) << __FUNCTION__ <<LOG_COLOR_ERROR<<": action[wait] just wait for " << duration_ms <<  " ms" <<LOG_COLOR_END;
				status_update = false;
			}
			if (duration_ms > 0) {
				duration_ms -= 10;
				pj_thread_sleep(10);
				continue;
			} else if (duration_ms == -1) {
				pj_thread_sleep(10);
				continue;
			}

			completed = true;
			LOG(logINFO) << __FUNCTION__ << ": completed";
		}
	}
}
