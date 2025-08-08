#include <iostream>

#include "text.hpp"

Text::Text()
{
    f_renderer = nullptr;
}

Text::Text(SDL_Renderer *renderer)
{
    f_renderer = renderer;
}

void Text::new_textfield(std::string field_name, std::string text, int x, int y, int w, int h, int size)
{
    if (f_renderer == nullptr) {
        std::cout << "Error: no renderer provided" << std::endl;
        return;
    }

    if (find_textfield(field_name) != nullptr) {
        delete_textfield(field_name);
    }

#if defined (__unix)
	std::string font_path = "res/fonts/default.otf";
#else 
	std::string font_path = "res\\fonts\\default.otf";
#endif
    
    TTF_Font* font = TTF_OpenFont(font_path.c_str(), size);
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), white);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(f_renderer, surf);
    SDL_Rect* rect = new SDL_Rect(x, y, w, h); 

    f_textfields.push_back(TextField(field_name, text, true, rect, tex));
    
    TTF_CloseFont(font);
}

void Text::activate_textfield(std::string field_name)
{
    TextField* curfield = find_textfield(field_name);

    if (curfield != nullptr) {
        curfield->active = true;
    }
}

void Text::deactivate_textfield(std::string field_name)
{
    TextField* curfield = find_textfield(field_name);

    if (curfield != nullptr) {
        curfield->active = false;
    }
}

void Text::delete_textfield(std::string field_name)
{
    for (auto it = f_textfields.begin(); it != f_textfields.end(); ++it) {
        if (it->field_name == field_name) {
            delete it->rectangle;
            SDL_DestroyTexture(it->texture);
            f_textfields.erase(it);
            break;
        }
    }
}

TextField* Text::find_textfield(std::string field_name)
{
    for (auto &field: f_textfields) {
        if (field.field_name == field_name)
            return &field;
    }
    return nullptr;
}

std::list<TextField> *Text::get_textfields()
{
    return &f_textfields;
}
