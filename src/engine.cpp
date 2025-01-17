#include <engine.hpp>
#include <platform.hpp>
namespace Aurora {
	Engine::Engine(int width, int height, const std::string & title) : mWidth(width), mHeight(height), mTitle(title), mRunning(false) {
		mPlatform = new Platform();
		mEventManager = new EventManager();
		mEventManager->mResizeX = width;
		mEventManager->mResizeY = height;
		mManager = ResourceManager::instance();

		
	}
	void Engine::start(const std::string& initialScene) {
		init(initialScene);
		mRunning = true;
		run();
	}

	void Engine::update(double dt) {
		mRenderer->update(dt);
		update_game(&mGame, dt);
	}

	void Engine::render() {

		render_game(&mGame, mRenderer);
		mPhysics->render(mRenderer);

	}

	void Engine::init(const std::string& initialScene) {
		mPlatform->create_window(mTitle.c_str(), mWidth, mHeight);
		mRenderer = new Renderer(mWidth, mHeight);
		mManager->load_defaults();
		mRenderer->init_default_resources();
		mRenderer->create_camera(*mEventManager);
		mPhysics = Physics::instance();
		mPhysics->debug = false;
		std::function<void()> shutdown = [=]() { mRunning = false; };
		mEventManager->subscribe_to_event(Intents::Shutdown, shutdown);
		mEventManager->subscribe_to_resize_event(std::move([=](int a, int b) {mPlatform->handle_resize(a, b); }));
		mEventManager->subscribe_to_event(Intents::Escape, std::move((std::function<void()>)[=]() {mPlatform->capture_mouse(!mPlatform->capture); mPlatform->capture = !mPlatform->capture; }));
		mEventManager->subscribe_to_event(Intents::Debug, std::move((std::function<void()>)[=]() {mRenderer->debug = !mRenderer->debug; }));
		mGame.load(initialScene, mEventManager);
	}

	void Engine::run() {
		double current_time = mPlatform->get_time();
		double accumulator = 0;
		double t = 0;
		double dt = 1.0/144.0;
		while (mRunning) {
			double new_time = mPlatform->get_time();
			double frame_time = new_time - current_time;
			if (frame_time > 0.25)
				frame_time = 0.25;
			current_time = new_time;
			accumulator += frame_time;
			mEventManager->poll_input();
			while (accumulator >= dt) {
				t += dt;
				accumulator -= dt;
				mPhysics->fixed_update(dt);
			}
			update(frame_time);
			mRenderer->clear_screen(true, true);
			render();
			mPlatform->swap_buffers();
			mEventManager->update_previous();
		}
	}
}