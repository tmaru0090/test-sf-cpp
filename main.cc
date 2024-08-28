#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <strstream>
#include <SFML/Graphics.hpp>
#include "zenity_dialog.hh"
#include <locale>
#include <codecvt>
#include "sol/sol.hpp"
#include <fstream>
#include <cmath>
const float PI = 3.14159265358979323846f;
sf::Vector2f bezier(float t,sf::Vector2f p0,sf::Vector2f p1,sf::Vector2f p2){
	return (1-t)*(1-t)*p0+2*(1-t)*t*p1+t*t*p2;
}
struct SaveData{
	std::string playerName;
	int playerState;
};
struct Menu{
	std::string name;
	sf::Vector2f pos;
	sf::Color color;
};

// 自身の環境に合わせたため、他コントローラー互換性配慮なし
enum CustomKey{
	Triangle, // △ボタン
	Circle,   // ○ボタン 
	Cross,	  // ☓ボタン 
	Square,   // □ボタン
	Unkown,   // 不明 
};
CustomKey getKeyType(unsigned int joystick){
	if(sf::Joystick::isConnected(joystick)){	
		if(sf::Joystick::isButtonPressed(0,3)){
			return CustomKey::Triangle;
		}
		if(sf::Joystick::isButtonPressed(0,2)){
			return CustomKey::Circle;
		}
		if(sf::Joystick::isButtonPressed(0,1)){
			return CustomKey::Cross;
		}
		if(sf::Joystick::isButtonPressed(0,0)){	
			return CustomKey::Square;
		}
	}
	return CustomKey::Unkown;
}
void getJPressCnt(unsigned int joystick,CustomKey type,int* i){
	if(sf::Joystick::isConnected(joystick)){
		if(getKeyType(joystick) == type){
			(*i)++;	
		}else{
			(*i)=0;
		}
	}
}
void getKeyPressCnt(sf::Keyboard::Key key,int*i){
	if(sf::Keyboard::isKeyPressed(key)){
		(*i)++;
	}else {
		(*i)=0;
	}
}
void getJPosition(unsigned int joystick,float& x,float& y){
	float xx,yy;
	xx = sf::Joystick::getAxisPosition(joystick,sf::Joystick::PovX);
	yy = sf::Joystick::getAxisPosition(joystick,sf::Joystick::PovY);
	if(xx == 100){
		x+=1;
	}
	if(xx == -100){
		x-=1;
	}
	if(xx == 0){
		x = 0;
	}
	if(yy == 100){
		y+=1;
	}
	if(yy == -100){
		y-=1;
	}
	if(yy == 0){
		y = 0;
	}
}

int saveGame(const std::string&fileName,const SaveData& data){
	std::ofstream out(fileName,std::ios::binary);
	if(out.is_open()){
		out.write(reinterpret_cast<const char*>(&data),sizeof(SaveData));
		out.close();
	}else{
		std::cerr << "Failed to open file" << std::endl;
		return -1;
	}
	return 0;
}
SaveData loadGame(const std::string& fileName){
	SaveData data;
	std::ifstream ifs(fileName,std::ios::binary);
	if(ifs.is_open()){
		ifs.read(reinterpret_cast<char*>(&data),sizeof(SaveData));
		ifs.close();
	}else{
		std::cerr << "Failed to open file" << std::endl;
		ifs.close();
	}
	return data;
}
int main(){
	sol::state lua;
	lua.open_libraries(sol::lib::base);
	// C++関数を登録 
	lua.new_enum<CustomKey>("CustomKey",{
		{"Cross",CustomKey::Cross},
		{"Circle",CustomKey::Circle},
		{"Square",CustomKey::Square},
		{"Triangle",CustomKey::Triangle},
		{"Unkown",CustomKey::Unkown},
	});
	lua.new_enum<Dialog>("Dialog",{
	{"Info",Dialog::Info},
	{"Question",Dialog::Question},
	{"Error",Dialog::Error},
	{"Warning",Dialog::Warning},
	{"Entry",Dialog::Entry},
	{"FileSelection",Dialog::FileSelection},
	{"Calendar",Dialog::Calendar},
	{"List",Dialog::List},
	});
	lua.set_function("getJPressCnt",&getJPressCnt);
	lua.set_function("ZDialog1",&ZDialog1);
	lua.set_function("ZDialog2",&ZDialog2);
	lua.script_file("main.lua");
	sol::function initialize = lua["initialize"];
	sol::function finalize = lua["finalize"];
	sol::function mainloop = lua["mainloop"];
	int joystick = 0;
	std::string musicPath = "";
	musicPath= ZDialog2(Dialog::FileSelection,"音声ファイルを選択してください");
	
	ZenityID id;
	sf::RenderWindow window(sf::VideoMode(640,480,32),"");
	sf::Vector2f center(window.getSize().x/2.0f,window.getSize().y/2.0f);
	float radius = 85.0f;
	sf::Font font;
	float x = 0;
	float y = 0;
/*
	float menuTargetX1 = 140;
	float menuTargetX2 = 100;
*/ 
	sf::Vector2f targetCenter = center;
	float targetRadius = radius;
	float menuTargetX1 =targetCenter.x+targetRadius;
	float menuTargetX2 = targetCenter.x - targetRadius;

	bool musicFlag = false;
	bool endFlag = false;
	bool saveFlag = false;
	bool loadFlag = false;
	std::vector<Menu>menu;
	sf::Text text("exit to press Escape or Joypad-X",font);
	sf::Text text2("ロードデータ: ",font);
	text2.setPosition(0,50);
	sf::Music music;
	std::string menuName[5] = {"New Game","Save","Load","Config","Quit"};
	SaveData gameData,loadData;
	int circleButtonPressCnt = 0;
	int crossButtonPressCnt = 0;
	int squareButtonPressCnt = 0;
	int triangleButtonPressCnt = 0;
	
	int upButtonPressCnt = 0;
	int downButtonPressCnt = 0;
	int leftButtonPressCnt = 0;
	int rightButtonPressCnt = 0;
	int escapeButtonPressCnt = 0;
	int zButtonPressCnt = 0;
	gameData.playerName = "takashi";
	gameData.playerState = -1;
	/*
	for(int i=0;i<sizeof(menuName)/sizeof(menuName[0]);i++){
		menu.push_back(Menu{
			menuName[i],
			sf::Vector2f(100,100+i*20),
			sf::Color(100+i*2,100+i*3,100+i*4)
		});
	}*/
	for(int i=0;i<sizeof(menuName)/sizeof(menuName[0]);i++){
		float angle = 2*PI*i/5;
		float x = center.x+radius*std::cos(angle);
		float y = center.y+radius*std::sin(angle);
		menu.push_back(Menu{
			menuName[i],
			sf::Vector2f(x,y),
			sf::Color(100+i*2,100+i*3,100+i*4)
		});
	}
	int menuState = 0;
	if(!font.loadFromFile("/usr/share/fonts/truetype/freefont/FreeSans.ttf")){
		ZDialog1(Dialog::Error,"フォントのロードに失敗しました");
		return -1;
	}
	if(!music.openFromFile(musicPath)){
		ZDialog1(Dialog::Error,"音声ファイルのロードに失敗しました");
		return -1;
	}
	initialize();
	while(window.isOpen()){
		sf::Event e;
		sf::Joystick::update();
		while(window.pollEvent(e)){
			if(e.type == sf::Event::Closed){
				window.close();
				break;
			}
		}
		window.clear();
		getKeyPressCnt(sf::Keyboard::Escape,&escapeButtonPressCnt);
		getKeyPressCnt(sf::Keyboard::Z,&zButtonPressCnt);
		getKeyPressCnt(sf::Keyboard::Up,&upButtonPressCnt);
		getKeyPressCnt(sf::Keyboard::Down,&downButtonPressCnt);
		getKeyPressCnt(sf::Keyboard::Left,&leftButtonPressCnt);
		getKeyPressCnt(sf::Keyboard::Right,&rightButtonPressCnt);
		
		getJPressCnt(0,CustomKey::Square,&squareButtonPressCnt);
		getJPressCnt(0,CustomKey::Cross,&crossButtonPressCnt);
		getJPressCnt(0,CustomKey::Circle,&circleButtonPressCnt);
		getJPressCnt(0,CustomKey::Triangle,&triangleButtonPressCnt);
		



		if(escapeButtonPressCnt == 1|| menuState == 4&&zButtonPressCnt == 1||menuState == 4&&circleButtonPressCnt == 1)  {
			id = ZDialog1(Dialog::Question,"本当に終了しますか？");
			if(id == ZenityID::Ok){
				endFlag = true;
			}else{
				endFlag = false;
			}
		}
		window.draw(text);
		if(endFlag){
			break;
		}
		if(musicFlag){
			music.play();
		}
		if(saveFlag){
			ZDialog1(Dialog::Info,"セーブしました");
			saveGame("save.dat",gameData);
		}
		if(loadFlag){
			ZDialog1(Dialog::Info,"ロードしました");
			loadData = loadGame("save.dat");
		}
		getJPosition(joystick,x,y);
		//std::cout << x << std::endl;
		//std::cout << y << std::endl;
		// 通常
		if(upButtonPressCnt == 1 || (y == -1) ){
			menuState = (menuState-1+menu.size())%menu.size();
		}
		if(downButtonPressCnt == 1 || (y == 1) ){
			menuState = (menuState+1) % menu.size();
		}
		
		// 長押し
		if( (y < -35) || (upButtonPressCnt > 35) ){
			if( ((int)y % 45) == 0 && (upButtonPressCnt%45) == 0){
				menuState = (menuState-1+menu.size())%menu.size();
			}
		}
		if( (y > 35) || (downButtonPressCnt) > 35 ){
			if( ((int)y % 45) == 0 && (downButtonPressCnt%45) == 0 ){
				menuState = (menuState+1) % menu.size();
			}
		}

		if(menuState == 0&&circleButtonPressCnt == 1 || menuState == 0&&zButtonPressCnt == 1){
			musicFlag = true;
		}else{
			musicFlag = false;
		}
                if(menuState == 1&&circleButtonPressCnt == 1 || menuState == 1&&zButtonPressCnt == 1){
			saveFlag = true;
		}else{
			saveFlag = false;
		}
		if(menuState == 2&&circleButtonPressCnt == 1 || menuState == 2&&zButtonPressCnt == 1){
			loadFlag = true;
		}else{
			loadFlag = false;
		}


		for(int i=0;i<menu.size();i++){	

			sf::Text text(menu[i].name,font);
			if(menuState == i){
				auto xx = (menuTargetX1-menu[i].pos.x)*0.08;
				menu[i].pos.x += xx;
			}else{
				auto xx = (menuTargetX2-menu[i].pos.x)*0.08;
				menu[i].pos.x += xx;
			}

			text.setPosition(menu[i].pos);
			text.setFillColor(menu[i].color);
			float angle = atan2(menu[i].pos.y-center.y,menu[i].pos.x-center.x)*180/PI;
			text.setRotation(angle);
			window.draw(text);
		}
		text2.setString(std::string("load data: ")+loadData.playerName+std::string(" ")+std::to_string(loadData.playerState));
		window.draw(text2);
		mainloop();
		window.display();
	}
	finalize();
}
