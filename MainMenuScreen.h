#ifndef MAINMENUSCREEN_H
#define MAINMENUSCREEN_H


#include <SOIL/SOIL.h>

#include <SDL/SDL_mixer.h>
#include <SDL/SDL_thread.h>

#include <ft2build.h>
#include <freetype/freetype.h>
#include <map>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "Screen.h"

using namespace glm;

#define NUM_BUTTON 5
#define NUM_LOGO 1
#define NUM_BTN_SETTING 2
#define NUM_LEADERBOARD_SCREEN 1
#define NUM_CREDIT_SCREEN 1

class MainMenuScreen :
	public Layar::Screen {
public:
	MainMenuScreen();
	~MainMenuScreen();
	virtual void Init();
	virtual void DeInit();
	virtual void Update(float deltaTime);
	virtual void Render();

private:
	GLuint BtnTexture[NUM_BUTTON], BtnHoverTexture[NUM_BUTTON], LogoTexture[NUM_LOGO], 
		BtnSettingTex[NUM_BTN_SETTING], BtnSettingHoverTex[NUM_BTN_SETTING], BtnSettingChoosenTex[NUM_BTN_SETTING],
		LeaderboardTex[NUM_LEADERBOARD_SCREEN], CreditScreenTex[NUM_CREDIT_SCREEN], CreditVAO, CreditVBO, LeaderboardVAO, LeaderboardVBO,
		BtnSettingVAO, BtnSettingVBO, BtnVAO, BtnVBO, LogoVAO, LogoVBO, LogoEBO, program, program2;
	float BtnWidth[NUM_BUTTON], BtnHeigt[NUM_BUTTON], BtnHoverWidth[NUM_BUTTON], 
		BtnHoverHeight[NUM_BUTTON], LogoWidth[NUM_LOGO], LogoHeight[NUM_LOGO],
		BtnSettingWidth[NUM_BTN_SETTING], BtnSettingHeight[NUM_BTN_SETTING], BtnSettingHoverWidth[NUM_BTN_SETTING], 
		BtnSettingHoverHeight[NUM_BTN_SETTING], BtnSettingChoosenWidth[NUM_BTN_SETTING], BtnSettingChoosenHeight[NUM_BTN_SETTING],
		LeaderboardHeight[NUM_LEADERBOARD_SCREEN], LeaderboardWidth[NUM_LEADERBOARD_SCREEN], CreditHeight[NUM_CREDIT_SCREEN], CreditWidth[NUM_CREDIT_SCREEN];
	int ActiveButtonIndex = 0;
	Status status;

	//Build Main Menu Screen
	void BuildLogo();
	void DrawLogo();
	void BuildBtn();
	void DrawBtn();

	//Build Button Screen
	int BtnActive = 0;
	void BuildBtnSetting();
	void DrawBtnSetting();

	//Build Leaderboard Screen
	void BuildLeaderboard();
	void DrawLeaderboard();

	//Build Credit Screen
	void BuildCredit();
	void DrawCredit();
};

#endif