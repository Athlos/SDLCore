// 717310 C++ SDL Framework

// This includes:
#include "game.h"

// Local includes:
#include "backbuffer.h"
#include "inputhandler.h"
#include "logmanager.h"
#include "sprite.h"
#include "animatedsprite.h"
#include "fmod.hpp"
#include "label.h"
#include "particleemitter.h"

// Library includes:
#include <cassert>
#include <SDL.h>
#include <stdio.h>
#include <cmath>
#include <ctime>
#include <vector>

// Static Members:
Game* Game::sm_pInstance = 0;

Game& Game::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new Game();
	}

	assert(sm_pInstance);

	return (*sm_pInstance);
}

void Game::DestroyInstance()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

Game::Game()
: m_pBackBuffer(0)
, m_pInputHandler(0)
, m_looping(true)
, m_executionTime(0)
, m_elapsedSeconds(0)
, m_frameCount(0)
, m_FPS(0)
, m_numUpdates(0)
, m_lastTime(0)
, m_lag(0)
, m_paused(0)
{

}

Game::~Game()
{
	delete m_pBackBuffer;
	m_pBackBuffer = 0;

	delete m_particles;
	m_particles = 0;

	delete m_pInputHandler;
	m_pInputHandler = 0;

	system->release();
}

/*
Initialises renderer, input, audio, player and other variables needed to begin
*/
bool Game::Initialise()
{
	//SET UP GRAPHICS AND INPUT
	const int width = 800;
	const int height = 600;

	// Load backbuffer
	m_pBackBuffer = new BackBuffer();
	if (!m_pBackBuffer->Initialise(width, height))
	{
		LogManager::GetInstance().Log("BackBuffer Init Fail!");
		return (false);
	}

	// Load input
	m_pInputHandler = new InputHandler();
	if (!m_pInputHandler->Initialise())
	{
		LogManager::GetInstance().Log("InputHandler Init Fail!");
		return (false);
	}

	// Seed rand
	srand(time(0));

	//SET UP ENTITIES AND VARIABLES
	//Sprite* pPlayerSprite = m_pBackBuffer->CreateSprite("assets\\wall.png");

	m_particles = new ParticleEmitter();

	//SET UP AUDIO
	system = NULL;

	result = FMOD::System_Create(&system);      // Create the main system object.
	result = system->init(32, FMOD_INIT_NORMAL, 0);    // Initialize FMOD.

	channel = 0;
	combatMusic = 0;
	m_musicChannel = 0;

	//result = system->createSound("assets\\error.wav", FMOD_DEFAULT, 0, &m_error);

	//result = m_towerPlacedAuto->setMode(FMOD_LOOP_OFF);

	//SET UP UI

	//electricity label
	//std::string elecString = "Electricity: " + std::to_string(m_electricity);
	//m_electricityLabel = new Label(elecString);
	//m_electricityLabel->SetColour(0, 0, 255, 0);
	//m_electricityLabel->SetBounds(0, 0, 128, 24);

	// Timings
	m_lastTime = SDL_GetTicks();
	m_lag = 0.0f;

	return (true);
}

bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f; // calculate step size

	// Check input
	assert(m_pInputHandler);
	m_pInputHandler->ProcessInput(*this);

	if (m_looping)
	{
		// Set up time values
		int current = SDL_GetTicks();
		float deltaTime = (current - m_lastTime) / 1000.0f;
		m_lastTime = current;

		m_executionTime += deltaTime;

		m_lag += deltaTime;

		while (m_lag >= stepSize)
		{
			Process(stepSize); // Process

			m_lag -= stepSize;

			++m_numUpdates;
		}

		Draw(*m_pBackBuffer); // Render
	}

	SDL_Delay(1);

	return (m_looping);
}

void Game::Process(float deltaTime)
{
	// Count total simulation time elapsed:
	m_elapsedSeconds += deltaTime;

	// Frame Counter
	if (m_elapsedSeconds > 1)
	{
		m_elapsedSeconds -= 1;
		m_FPS = m_frameCount;
		m_frameCount = 0;
	}

	// Check if game is paused
	if (m_paused)
	{
		return;
	}

	// Update the game world simulation:


	//std::vector<Bullet*>::iterator iter = m_towerBullets.begin();

	//while (iter != m_towerBullets.end())
	//{
	//	Bullet* current = *iter;
	//	if (current->IsDead() || static_cast<int>(current->GetPositionY()) <= 0)
	//	{
	//		iter = m_towerBullets.erase(iter);
	//		delete current;
	//	}
	//	else
	//	{
	//		current->Process(deltaTime);
	//		if (static_cast<int>(current->GetPositionY()) <= 0)
	//		{
	//			LogManager::GetInstance().Log("bullet at zero");
	//		}
	//		m_particles.SpawnNewParticles(current->GetPositionX() + 4, current->GetPositionY() + 8, 1, m_pBackBuffer, current->GetParticleType());
	//		iter++;
	//	}
	//}

	m_particles->Process(deltaTime); // Process particles

	system->update(); // Update system
}

void Game::Draw(BackBuffer& backBuffer)
{
	++m_frameCount;

	backBuffer.Clear();

	m_particles->Draw(backBuffer);

	//Draw items in all vectors
	
	//DRAW UI ELEMENTS

	backBuffer.Present();
}

void Game::Quit()
{
	m_looping = false;
}

void Game::Pause(bool pause)
{
	m_paused = pause;
	m_musicChannel->setPaused(pause);
}

bool Game::IsPaused()
{
	return (m_paused);
}