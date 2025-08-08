#pragma once
#include <string>
#include <list>

#if defined(__unix)
	#include "SDL2/SDL_image.h"
    #include "SDL2/SDL_ttf.h"
#else
	#include "SDL_image.h"
    #include "SDL_ttf.h"

#endif

struct TextField {
    std::string field_name;
    std::string text;
    bool active = true;
    SDL_Rect* rectangle;
    SDL_Texture* texture;
};

class Text {
public:
    Text();
    Text(SDL_Renderer* renderer);

    void new_textfield(std::string field_name, std::string text, int x, int y, int w, int h, int size);
    void activate_textfield(std::string field_name);
    void deactivate_textfield(std::string field_name);
    void delete_textfield(std::string field_name);

    TextField* find_textfield(std::string field_name);
    std::list<TextField>* get_textfields();

private:
    std::list<TextField> f_textfields;
    SDL_Renderer* f_renderer;
};