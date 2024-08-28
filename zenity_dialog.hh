#pragma once
#include <string>
#include <variant>
#include <vector>

enum Dialog{
	Info,
	Question,
	Error,
	Warning,
	Entry,
	FileSelection,
	Calendar,
	List,
};
enum ZenityID{
	Ok,
	Cancel,
};


ZenityID ZDialog1(Dialog dialog,const std::variant<std::string,std::vector<std::string>> value);

std::string ZDialog2(Dialog dialog,const std::variant<std::string,std::vector<std::string>> value);
