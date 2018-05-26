#include "MainMenuScreen.h"

MainMenuScreen::MainMenuScreen()
{
}

MainMenuScreen::~MainMenuScreen()
{
	if (Mix_Playing(sfx_channel) == 0) {
		Mix_FreeChunk(sound);
	}
	if (music != NULL || music_vacuum != NULL) {
		Mix_FreeMusic(music);
		Mix_FreeMusic(music_vacuum);
	}
	Mix_CloseAudio();

}

void MainMenuScreen::Init()
{

	BuildLogo();
	BuildBtn();
	BuildBtnSetting();
	BuildLeaderboard();
	BuildCredit();
	BuildObstacle();
	this->program = BuildShader("shader.vert", "shader.frag");
	InputMapping("SelectButton", SDLK_RETURN);
	InputMapping("NextButton", SDLK_DOWN);
	InputMapping("PrevButton", SDLK_UP);
	InputMapping("BackButton", SDLK_ESCAPE);
	this->status = Status::MAIN_MENU;

	//Init Audio
	InitAudio();
}

void MainMenuScreen::DeInit()
{
}

void MainMenuScreen::Update(float deltaTime)
{
	if (Status::MAIN_MENU == status)
	{
		//------MUSIC PLAYING-------
		if (Mix_PlayingMusic() == 0)
		{
			//Play the music
			Mix_PlayMusic(music, -1);
			SDL_Delay(150);
		}

		if (IsKeyDown("SelectButton")) {
			if (ActiveButtonIndex == 0) {

				//SFX_Klik (Msh bug jika klik2x)
				sfx_channel = Mix_PlayChannel(-1, sfx_klik, 0);
				if (sfx_channel == -1) {
					Err("Unable to play WAV file: " + string(Mix_GetError()));
				}

				//Fade out music
				Mix_FadeOutMusic(3000);
				SDL_Delay(150);

				this->status = Status::RUN;
			}
			if (ActiveButtonIndex == 1) {
				this->status = Status::LEADERBOARD;
			}
			if (ActiveButtonIndex == 2) {
				this->status = Status::SETTING;
			}
			if (ActiveButtonIndex == 3) {
				this->status = Status::CREDIT;
			}
			if (ActiveButtonIndex == 4) {
				SDL_Quit();
				exit(0);
			}
		}

		if (IsKeyUp("NextButton")) {
			if (ActiveButtonIndex < NUM_BUTTON - 1) {
				ActiveButtonIndex = ActiveButtonIndex + 1;

				//SFX_Clik
				sfx_channel = Mix_PlayChannel(-1, sound, 0);
				if (sfx_channel == -1) {
					Err("Unable to play WAV file: " + string(Mix_GetError()));
				}

				SDL_Delay(150);
			}
		}

		if (IsKeyUp("PrevButton")) {
			if (ActiveButtonIndex > 0) {
				ActiveButtonIndex = ActiveButtonIndex - 1;

				//SFX_Clik
				sfx_channel = Mix_PlayChannel(-1, sound, 0);
				if (sfx_channel == -1) {
					Err("Unable to play WAV file: " + string(Mix_GetError()));
				}

				SDL_Delay(150);
			}
		}
		if (IsKeyUp("BackButton")) {
			this->status = Status::MAIN_MENU;
		}
	}
	if (Status::RUN == status)
	{
		//----Paying Vacuum BGM-----
		if (Mix_PlayingMusic() == 0)
		{
			//Play the music
			Mix_PlayMusic(music_vacuum, -1);
			SDL_Delay(150);
		}

		UpdatePlayerSpriteAnim(deltaTime);
		ControlPlayerSprite(deltaTime);

		if (IsKeyUp("BackButton")) {
			this->status = Status::MAIN_MENU;
		}
	}
	if (Status::LEADERBOARD == status)
	{
		if (IsKeyUp("BackButton")) {
			this->status = Status::MAIN_MENU;
		}
	}
	if (Status::SETTING == status)
	{
		if (IsKeyDown("SelectButton"))
		{
			if (BtnActive == 0)
			{

			}
			if (BtnActive == 1)
			{

			}
		}
		if (IsKeyUp("NextButton")) {
			if (BtnActive < NUM_BTN_SETTING - 1) {
				BtnActive = BtnActive + 1;
				SDL_Delay(150);
			}
		}

		if (IsKeyUp("PrevButton")) {
			if (BtnActive > 0) {
				BtnActive = BtnActive - 1;
				SDL_Delay(150);
			}
		}
		if (IsKeyUp("BackButton")) {
			this->status = Status::MAIN_MENU;
		}
	}
	if (Status::CREDIT == status)
	{
		if (IsKeyUp("BackButton")) {
			this->status = Status::MAIN_MENU;
		}
	}
}

//------------------AUDIO--------------------
void MainMenuScreen::InitAudio() {
	int flags = MIX_INIT_MP3 | MIX_INIT_FLAC | MIX_INIT_OGG;
	if (flags != Mix_Init(flags)) {
		Err("Unable to initialize mixer: " + string(Mix_GetError()));
	}

	int audio_rate = 22050; Uint16 audio_format = AUDIO_S16SYS; int audio_channels = 2; int audio_buffers = 4096;

	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
		Err("Unable to initialize audio: " + string(Mix_GetError()));
	}


	music = Mix_LoadMUS("BGM_Menu.wav");
	if (music == NULL) {
		Err("Unable to load Music file: " + string(Mix_GetError()));
	}

	music_vacuum = Mix_LoadMUS("BGM_Vacuum.wav");
	if (music_vacuum == NULL) {
		Err("Unable to load Music file: " + string(Mix_GetError()));
	}

	sound = Mix_LoadWAV("SFX_Clik.wav");
	if (sound == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}

	sfx_klik = Mix_LoadWAV("SFX_Klik.wav");
	if (sfx_klik == NULL) {
		Err("Unable to load WAV file: " + string(Mix_GetError()));
	}

}

/*void MainMenuScreen::AddInputs() {
InputMapping("Quit", SDLK_ESCAPE);
InputMapping("BGM", SDLK_m);
InputMapping("SFX", SDLK_s);
}*/


//---------OBSTACLE UI-----------------
void MainMenuScreen::Render()
{
	//Setting viewport
	glViewport(0, 0, GetScreenWidth(), GetScreenHeight());

	//Clear the color and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set the background color
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	// Set orthographic projection
	mat4 projection;
	projection = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, value_ptr(projection));


	if (Status::MAIN_MENU == status)
	{
		DrawLogo();
		DrawBtn();
	}
	if (Status::RUN == status)
	{
		DrawObstacle();
		mat4 projection2;
		projection2 = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(this->program2, "projection"), 1, GL_FALSE, value_ptr(projection2));
	}
	if (Status::SETTING == status)
	{
		DrawBtnSetting();
	}
	if (Status::LEADERBOARD == status)
	{
		DrawLeaderboard();
	}
	if (Status::CREDIT == status)
	{
		DrawCredit();
	}
}

void MainMenuScreen::BuildObstacle()
{
	this->program2 = BuildShader("spriteAnim.vert", "spriteAnim.frag");
	UseShader(this->program2);

	// Load and create a texture 
	glGenTextures(1, &textureObstacle);
	glBindTexture(GL_TEXTURE_2D, textureObstacle); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

												   // Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Load, create texture 
	int width, height;
	unsigned char* image = SOIL_load_image("crate.png", &width, &height, 0, SOIL_LOAD_RGBA);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

									 // Set up vertex data (and buffer(s)) and attribute pointers
	frame_width = ((float)width) / NUM_FRAMES;
	frame_height = (float)height;
	GLfloat vertices[] = {
		// Positions   // Colors           // Texture Coords
		1,  1, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Bottom Right
		1,  0, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f, // Top Right
		0,  0, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f, // Top Left
		0,  1, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f  // Bottom Left 
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 3, 2, 1
	};

	glGenVertexArrays(1, &VAOObstacle);
	glGenBuffers(1, &VBOObstacle);
	glGenBuffers(1, &EBOObstacle);

	glBindVertexArray(VAOObstacle);

	glBindBuffer(GL_ARRAY_BUFFER, VBOObstacle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOObstacle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

						  // Set orthographic projection
	mat4 projection2;
	projection2 = ortho(0.0f, static_cast<GLfloat>(GetScreenWidth()), static_cast<GLfloat>(GetScreenHeight()), 0.0f, -1.0f, 1.0f);
	UseShader(this->program2);
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "projection"), 1, GL_FALSE, value_ptr(projection2));
	xpos = GetScreenWidth();
	ypos = GetScreenHeight() / 2;
}

void MainMenuScreen::DrawObstacle()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, textureObstacle);

	// Activate shader
	UseShader(this->program2);
	glUniform1i(glGetUniformLocation(this->program2, "ourTexture"), 10);

	mat4 model;
	// Translate sprite along x-axis
	model = translate(model, vec3(xpos, ypos, 0.0f));
	// Scale sprite 
	model = scale(model, vec3(frame_width * 8, frame_height, 1));
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "model"), 1, GL_FALSE, value_ptr(model));

	// Draw sprite
	glBindVertexArray(VAOObstacle);
	glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
	glDisable(GL_BLEND);
}

void MainMenuScreen::UpdatePlayerSpriteAnim(float deltaTime)
{
	// Update animation
	frame_dur += deltaTime;

	if (frame_dur > FRAME_DUR) {
		frame_dur = 0;
		if (frame_idx == NUM_FRAMES - 1) frame_idx = 0;  else frame_idx++;

		// Pass frameIndex to shader
		GLint location = glGetUniformLocation(this->program2, "frameIndex");
		UseShader(this->program2);
		glUniform1i(location, frame_idx);
	}
}

void MainMenuScreen::ControlPlayerSprite(float deltaTime)
{
	xpos += deltaTime * -0.3f;

	if (xpos < 0) {
		xpos = GetScreenWidth();
		ypos = (GetScreenHeight() / 2) + (rand() % (GetScreenHeight() / 2));
		if (ypos > GetScreenHeight()) {
			ypos = GetScreenHeight() - 100;
		}
		//ypos = 30 + rand() % (GetScreenHeight() + 400);
	}
	mat4 model;
	model = translate(model, vec3(xpos - frame_width, ypos, 0.0f));

	// Rotate sprite at origin
	model = translate(model, vec3(0.5f * frame_width, 0.5f * frame_height, 0.0f));
	model = translate(model, glm::vec3(-0.5f * frame_width, -0.5f * frame_height, 0.0f));

	// Scale sprite 
	model = scale(model, vec3(frame_width, frame_height, 1));


	UseShader(this->program2);
	glUniformMatrix4fv(glGetUniformLocation(this->program2, "model"), 1, GL_FALSE, value_ptr(model));
}


//-------------MAIN MENU UI------------------
void MainMenuScreen::BuildLogo()
{
	// Load and create a texture 
	string logo[NUM_LOGO] = { "Title.png" };
	
	glGenTextures(1, &LogoTexture[0]);

	for (int i = 0; i < NUM_LOGO; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, LogoTexture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

													 // Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(logo[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		LogoWidth[i] = (float)width;
		LogoHeight[i] = (float)height; // Unbind texture when done, so we won't accidentily mess up our texture.

									 // Set up vertex data (and buffer(s)) and attribute pointers
		GLfloat vertices[] = {
			// Positions	// Texture Coords
			1,  1,			1.0f, 1.0f, // Bottom Right
			1,  0,			1.0f, 0.0f, // Top Right
			0,  0,			0.0f, 0.0f, // Top Left
			0,  1,			0.0f, 1.0f  // Bottom Left 
		};


		glGenVertexArrays(1, &LogoVAO);
		glGenBuffers(1, &LogoVBO);
		glBindVertexArray(LogoVAO);
		glBindBuffer(GL_ARRAY_BUFFER, LogoVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void MainMenuScreen::DrawLogo()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int texture_id[NUM_LOGO] = { GL_TEXTURE0 };
	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "text"), 0);

	glBindVertexArray(LogoVAO);
	for (int i = 0; i < NUM_LOGO; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, LogoTexture[i]);
		glUniform1i(glGetUniformLocation(this->program, "ourTexture"), i);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth()) / 4.65f, (i) * 100, 0.0f));
		model = scale(model, vec3(LogoWidth[i], LogoHeight[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);

}

void MainMenuScreen::BuildBtn()
{
	string buttons[NUM_BUTTON] = { "run_hover.png", "leaderboard_hover.png", "setting_hover.png", "credit_hover.png", "quit_hover.png" }; 
	string hover_buttons[NUM_BUTTON] = { "run.png", "leaderboard.png", "setting.png", "credit.png", "quit.png" };

	glGenTextures(5, &BtnTexture[0]);
	glGenTextures(5, &BtnHoverTexture[0]);

	for (int i = 0; i < NUM_BUTTON; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, BtnTexture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

													 // Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		BtnWidth[i] = (float)width;
		BtnHeigt[i] = (float)height;

		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, BtnHoverTexture[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

														  // Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(hover_buttons[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		BtnHoverWidth[i] = (float)width;
		BtnHoverHeight[i] = (float)height;
	}

	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &BtnVAO);
	glGenBuffers(1, &BtnVBO);
	glBindVertexArray(BtnVAO);
	glBindBuffer(GL_ARRAY_BUFFER, BtnVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MainMenuScreen::DrawBtn()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_BUTTON] = { GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5 };
	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "text"), 0);

	glBindVertexArray(BtnVAO);
	for (int i = 0; i < NUM_BUTTON; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (ActiveButtonIndex == i) ? BtnHoverTexture[i] : BtnTexture[i]);
		glUniform1i(glGetUniformLocation(this->program, "ourTexture"), i + 1);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth()) / 2.8f, (i + 1) * 115, 0.0f));
		model = scale(model, vec3(BtnWidth[i] / 2, BtnHeigt[i] / 2, 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void MainMenuScreen::BuildBtnSetting()
{
	string btnSetting[NUM_BTN_SETTING] = {  "bgm_hover.png", "sfx_hover.png" };
	string btnSettingHover[NUM_BTN_SETTING] = { "bgm.png", "sfx.png" };
	string btnSettingChoosen[NUM_BTN_SETTING] = { "bgm_choosen.png", "sfx_choosen.png" };

	glGenTextures(2, &BtnSettingTex[0]);
	glGenTextures(2, &BtnSettingHoverTex[0]);
	glGenTextures(2, &BtnSettingChoosenTex[0]);

	for (int i = 0; i < NUM_BTN_SETTING; i++)
	{
		glBindTexture(GL_TEXTURE_2D, BtnSettingTex[i]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture Btn Setting
		int width, height;
		unsigned char* image = SOIL_load_image(btnSetting[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		BtnSettingWidth[i] = (float)width;
		BtnSettingHeight[i] = (float)height;

		// Load and create a texture
		glBindTexture(GL_TEXTURE_2D, BtnSettingHoverTex[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(btnSettingHover[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		BtnSettingHoverWidth[i] = (float)width;
		BtnSettingHoverHeight[i] = (float)height;

		//Btn Setting Choosen Texture
		glBindTexture(GL_TEXTURE_2D, BtnSettingChoosenTex[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		image = SOIL_load_image(btnSettingChoosen[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		BtnSettingChoosenWidth[i] = (float)width;
		BtnSettingChoosenHeight[i] = (float)height;

		GLfloat vertices[] = {
			// Positions	// Texture Coords
			1,  1,			1.0f, 1.0f, // Bottom Right
			1,  0,			1.0f, 0.0f, // Top Right
			0,  0,			0.0f, 0.0f, // Top Left
			0,  1,			0.0f, 1.0f  // Bottom Left 
		};


		glGenVertexArrays(1, &BtnSettingVAO);
		glGenBuffers(1, &BtnSettingVBO);
		glBindVertexArray(BtnSettingVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BtnSettingVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void MainMenuScreen::DrawBtnSetting()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_BTN_SETTING] = { GL_TEXTURE6, GL_TEXTURE7 };
	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "text"), 0);

	glBindVertexArray(BtnSettingVAO);
	for (int i = 0; i < NUM_BTN_SETTING; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, (BtnActive == i) ? BtnSettingHoverTex[i] : BtnSettingTex[i]);
		glUniform1i(glGetUniformLocation(this->program, "ourTexture"), i + 6 );

		mat4 model;
		model = translate(model, vec3((GetScreenWidth()) / 3.25f, (i + 1) * 120, 0.0f));
		model = scale(model, vec3(BtnSettingWidth[i], BtnSettingHeight[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void MainMenuScreen::BuildLeaderboard()
{
	string leaderboardScreen[NUM_LEADERBOARD_SCREEN] = { "leaderboardScreen.png" };

	glGenTextures(1, &LeaderboardTex[0]);

	for (int i = 0; i < NUM_LEADERBOARD_SCREEN; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, LeaderboardTex[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

													 // Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(leaderboardScreen[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		LeaderboardWidth[i] = (float)width;
		LeaderboardHeight[i] = (float)height;
	}
	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &LeaderboardVAO);
	glGenBuffers(1, &LeaderboardVBO);
	glBindVertexArray(LeaderboardVAO);
	glBindBuffer(GL_ARRAY_BUFFER, LeaderboardVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MainMenuScreen::DrawLeaderboard()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_LEADERBOARD_SCREEN] = { GL_TEXTURE8 };
	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "text"), 0);

	glBindVertexArray(LeaderboardVAO);
	for (int i = 0; i < NUM_LEADERBOARD_SCREEN; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, LeaderboardTex[i]);
		glUniform1i(glGetUniformLocation(this->program, "ourTexture"), i + 8);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth()) / 6.0f, (i + 1) * 0, 0.0f));
		model = scale(model, vec3((GetScreenWidth()/1.5f), LeaderboardHeight[i], 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

void MainMenuScreen::BuildCredit()
{
	string creditScreen[NUM_CREDIT_SCREEN] = { "creditScreen.png" };

	glGenTextures(1, &CreditScreenTex[0]);

	for (int i = 0; i < NUM_CREDIT_SCREEN; i++) {
		// Load and create a texture 
		glBindTexture(GL_TEXTURE_2D, CreditScreenTex[i]); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object

														 // Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Load, create texture 
		int width, height;
		unsigned char* image = SOIL_load_image(creditScreen[i].c_str(), &width, &height, 0, SOIL_LOAD_RGBA);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

										 // Set up vertex data (and buffer(s)) and attribute pointers
		CreditWidth[i] = (float)width;
		CreditHeight[i] = (float)height;
	}
	GLfloat vertices[] = {
		// Positions	// Texture Coords
		1,  1,			1.0f, 1.0f, // Bottom Right
		1,  0,			1.0f, 0.0f, // Top Right
		0,  0,			0.0f, 0.0f, // Top Left
		0,  1,			0.0f, 1.0f  // Bottom Left 
	};


	glGenVertexArrays(1, &CreditVAO);
	glGenBuffers(1, &CreditVBO);
	glBindVertexArray(CreditVAO);
	glBindBuffer(GL_ARRAY_BUFFER, CreditVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void MainMenuScreen::DrawCredit()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Bind Textures using texture units
	int texture_id[NUM_CREDIT_SCREEN] = { GL_TEXTURE9 };
	// Activate shader
	UseShader(this->program);
	glUniform1i(glGetUniformLocation(this->program, "text"), 0);

	glBindVertexArray(CreditVAO);
	for (int i = 0; i < NUM_LEADERBOARD_SCREEN; i++) {

		glActiveTexture(texture_id[i]);
		glBindTexture(GL_TEXTURE_2D, CreditScreenTex[i]);
		glUniform1i(glGetUniformLocation(this->program, "ourTexture"), i + 9);

		mat4 model;
		model = translate(model, vec3((GetScreenWidth()) / 3.5f, (i + 1) * 0, 0.0f));
		model = scale(model, vec3(CreditWidth[i]/1.52f, CreditHeight[i]/1.52f, 1));
		glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, value_ptr(model));

		glDrawArrays(GL_QUADS, 0, 4);
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
}

int main(int argc, char** argv) {

	Layar::Screen &mainScreen = MainMenuScreen();
	mainScreen.Start("RUN BARN v1.0", 1080, 720, false, WindowFlag::WINDOWED, 60, 1);

	return 0;
}