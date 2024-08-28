#include "zenity_dialog.hh"
#include <strstream>
#include <iostream>
#include <array>
#include <cstdio>
#include <stdexcept>
#include <memory>
std::string exec(const char*cmd){
	std::array<char,128>buf;
	std::string res;
	std::unique_ptr<FILE,decltype(&pclose)>
	pipe(popen(cmd,"r"),pclose);
	if(!pipe){
		throw std::runtime_error("popen() falied!");
	}
	while(fgets(buf.data(),buf.size(),pipe.get()) != nullptr){
		res += buf.data();
		if(!res.empty()&&res.back() == '\n'){
			res.pop_back();
		}
	}
	return res;
}
std::string toDialogCmdString(Dialog dialog){
	std::string s;
	switch(dialog){
		case Info: 
			s = "--info";
			break;
		case Question: 
			s = "--question";
			break;
		case Error:
			s = "--error";
			break;
		case Warning:
			s = "--warning";
			break;
		case Entry:
			s = "--entry";
			break;
		case FileSelection: 
			s = "--file-selection";
			break;
		case Calendar:
			s = "--calendar";
			break;
		case List: 
			s = "--list";
			break;
	}
	return s;
}
template <typename T>
ZenityID toZenityID(T value){}
template <>
ZenityID toZenityID<int>(int value){
	switch (value) {
		case 0: 
			return ZenityID::Ok;
		case 1:
			return ZenityID::Cancel;
	}
}
ZenityID ZDialog1(Dialog dialog,const std::variant<std::string,std::vector<std::string>> value){
	std::string cmdName = "zenity";
	std::string cmd = "";
	std::ostrstream oss;
	oss << cmdName << " " << toDialogCmdString(dialog) << " ";
	switch(dialog){
		case Info: 
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Question: 
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Error:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Warning:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Entry:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case FileSelection: 
			oss << "--title=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Calendar:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		// 未実装
		case List:
			/*
			for(auto&&v:std::get<std::vector<std::string>>(value)){
				oss << v << " ";
			}*/
			break;
	}
	cmd = oss.str();
	//std::cout << cmd << std::endl;
	int cmdRes = system(cmd.c_str());
	return toZenityID<int>(cmdRes);
}

std::string ZDialog2(Dialog dialog,const std::variant<std::string,std::vector<std::string>> value){
	std::string cmdName = "zenity";
	std::string cmd = "";
	std::ostrstream oss;
	oss << cmdName << " " << toDialogCmdString(dialog) << " ";
	switch(dialog){
		case Info: 
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Question: 
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Error:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Warning:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Entry:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case FileSelection: 
			oss << "--title=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		case Calendar:
			oss << "--text=" << "'" <<  std::get<std::string>(value) << "'";
			break;
		// 未実装
		case List:
			/*
			for(auto&&v:std::get<std::vector<std::string>>(value)){
				oss << v << " ";
			}*/
			break;
	}
	cmd = oss.str();
	//std::cout << cmd << std::endl;
	return exec(cmd.c_str());
}

