#pragma once
template <typename Single>
class Singleton{
protected:
	Singleton(){}
public:
	virtual ~Singleton(){}
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	static Single* getInstance(){
		static Single single;
		return &single;
	}
};

