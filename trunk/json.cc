
#include "json.h"
#include <sstream>

std::string json::to_string(const value &v) {
	if(!is_string(v) && !is_bool(v) && !is_number(v)) {
		throw invalid_type_cast();
	}
	return v.string_;
}

std::string json::to_string(const json::null_t &) {
	return "null";
}

bool json::to_bool(const value &v) {
	if(!is_bool(v)) {
		throw invalid_type_cast();
	}
	return v.string_ == "true";
}

double json::to_number(const value &v) {
	if(!is_number(v)) {
		throw invalid_type_cast();
	}
	return strtod(v.string_.c_str(), 0);
}

size_t json::size(const value &v) {
	if(is_array(v)) {
		return v.array_->values_.size();
	}
	
	if(is_object(v)) {
		return v.object_->values_.size();
	}
	
	return 0;
}

bool json::has_key(const value &v, const std::string &key) {
	if(is_object(v)) {
		return v.object_->values_.find(key) != v.object_->values_.end();
	}
	return false;
}

boost::unordered_set<std::string> json::keys(const value &v) {
	
	boost::unordered_set<std::string> keys;
	if(is_object(v)) {
	
		const json_object::map_type &map = v.object_->values_;
	
		for(json_object::map_type::const_iterator it = map.begin(); it != map.end(); ++it) {
			keys.insert(it->first);
		}		
	}
	
	return keys;
}


json::value json::parse(std::istream &is) {
	return parse((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
}

json::value json::parse(const std::string &s) {
	return parse(s.begin(), s.end());
}

std::vector<uint8_t> json::detail::ucs2_to_utf8(uint16_t cp) {
	std::vector<uint8_t> utf8;
	
	if(cp < 0x80) {
		utf8.push_back(static_cast<uint8_t>(cp));
	} else if(cp < 0x0800) {
		uint8_t ch[2];
		ch[0] = 0xc0 | ((cp >> 6) & 0x1f);
		ch[1] = 0x80 | (cp & 0x3f);
		utf8.push_back(static_cast<uint8_t>(ch[0]));
		utf8.push_back(static_cast<uint8_t>(ch[1]));
	} else {
		uint8_t ch[3];
		ch[0] = 0xe0 | ((cp >> 6) & 0x0f);
		ch[1] = 0x80 | ((cp >> 6) & 0x3f);
		ch[2] = 0x80 | (cp & 0x3f);
		utf8.push_back(static_cast<uint8_t>(ch[0]));
		utf8.push_back(static_cast<uint8_t>(ch[1]));
		utf8.push_back(static_cast<uint8_t>(ch[2]));
	}
	
	return utf8;
}

std::string json::to_json_string(const value &v) {
	std::stringstream ss;
	
	if(is_string(v)) {
		ss << '"' << to_string(v) << '"';
	}
	
	if(is_number(v)) {
		ss << to_string(v);
	}
	
	if(is_null(v)) {
		ss << "null";
	}
	
	if(is_bool(v)) {
		ss << (to_bool(v) ? "true" : "false");
	}
	
	if(is_object(v)) {
		ss << "{";
		boost::unordered_set<std::string> k = keys(v);
		if(!k.empty()) {
			boost::unordered_set<std::string>::const_iterator it = k.begin();
			
			ss << '"' << *it << '"' << ':' << to_json_string(v[*it]);
			++it;
			for(;it != k.end(); ++it) {
				ss << ',' << '"' << *it << '"' << ':' << to_json_string(v[*it]);
			}
			
		}
		ss << "}";
	}
	
	if(is_array(v)) {
		ss << "[";
		if(size(v) != 0) {
			size_t i = 0;
			
			ss << to_json_string(v[i++]);
			
			for(;i != size(v); ++i) {
				ss << ',' << to_json_string(v[i]);
			}
			
		}
		ss << "]";
	}

	return ss.str();
}
