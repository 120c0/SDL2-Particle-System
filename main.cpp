#include <iostream>
#include <vector>
#include <string.h>
#include <random>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

SDL_Texture *particles_image[6];
std::random_device rd;
std::default_random_engine generator(rd());

std::uniform_real_distribution<float> ffx(0.01, 2);
std::uniform_real_distribution<float> ffy(0.01, 3);

class Particle
{
  public:
    Particle(SDL_FRect r)
      : rect {r}, frame {0.f}, life_time {std::rand() % 2000 + 20}, angle {std::rand() % 360 + 2}, acy {-ffy(generator)}, fx {ffx(generator)}
    {
      tick_begin = SDL_GetTicks();  
    }
    void update()
    {
      if(tick_begin + ((life_time + 1) / 6) * frame < SDL_GetTicks()) frame++;
      rect.x += std::cos(angle) * fx;
      rect.y += (acy += 0.1f);
    }
    bool isDead()
    {
      return tick_begin + life_time < SDL_GetTicks();
    }
    void render(SDL_Renderer *renderer)
    {
      if(!isDead())
        SDL_RenderCopyF(renderer, particles_image[(int) std::round(frame)], NULL, &rect);
    }

  private:
    SDL_FRect rect;
    int tick_begin, life_time, angle;
    float frame, acy, fx;
};

int main(int argc, char const *argv[])
{
    SDL_Window *window = SDL_CreateWindow("SDL2 - Particle System", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
  SDL_Event event;
  
  SDL_Texture *target = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

  bool window_is_open = window && renderer;

  for(int i = 0; i < 6; i++)
  {
    SDL_Surface *surface = IMG_Load(("images/particles/p/" + std::to_string(i) + ".png").c_str());

    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    
    particles_image[i] = SDL_CreateTextureFromSurface(renderer, surface);
  }

  int mx, my;
  std::vector<Particle> parts;

  while(window_is_open)
  {
    SDL_GetMouseState(&mx, &my);
    while(SDL_PollEvent(&event))
    {
      if(event.type == SDL_QUIT)
      {
        window_is_open = false;
      }
    }
    for(int i = 0; i < 30; i++)
      parts.push_back(Particle({(float)mx / 2.f, (float)my / 2.f, 5.f, 5.f}));

    SDL_SetRenderTarget(renderer, target);
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xff);
    SDL_RenderClear(renderer);

   
    int index = 0; 
    for(auto &p : parts)
    {
      p.update();
      p.render(renderer);
      if(p.isDead())
        parts.erase(parts.begin() + index);

      index++;
    }
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, target, NULL, NULL); 
    SDL_RenderPresent(renderer);
    
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);

  return EXIT_SUCCESS;
}
