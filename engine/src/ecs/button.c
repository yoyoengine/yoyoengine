/*
    This file is a part of yoyoengine. (https://github.com/yoyolick/yoyoengine)
    Copyright (C) 2024  Ryan Zmuda

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <yoyoengine/yoyoengine.h>

/*
    Old SCDG impl is practically useless, but in repo history can still be found before this commit.
    It had overlaying a text and image texture though, which could be useful in the future.
*/

void ye_add_button_component(struct ye_entity *entity, struct ye_rectf rect){
    struct ye_component_button *button = malloc(sizeof(struct ye_component_button));
    button->active = true;
    button->relative = false;
    button->rect = rect;

    // private state
    button->is_hovered = false;
    button->is_clicked = false;

    entity->button = button;
    ye_entity_list_add(&button_list_head, entity);
}

void ye_remove_button_component(struct ye_entity *entity){
    free(entity->button);
    entity->button = NULL;
    ye_entity_list_remove(&button_list_head, entity);
}

void ye_system_button(SDL_Event event){
    // if(event.type != SDL_MOUSEMOTION && event.type != SDL_MOUSEBUTTONDOWN && event.type != SDL_MOUSEBUTTONUP){
    //     return;
    // }

    // /*
    //     For each button in the button list, get its position (accounting for relativity)
    //     then proceed to check if the mouse is hovering over it and if it has been clicked on.
    // */

    // int mouseX, mouseY; SDL_GetMouseState(&mouseX, &mouseY);
    // bool clicking = (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP);
    
    // // printf("MouseX: %d, MouseY: %d\n", mouseX, mouseY);
    // // printf("Clicking: %d\n", clicking);

    // struct ye_entity_node *itr = button_list_head;
    // while(itr != NULL){
    //     struct ye_entity *entity = itr->entity;
    //     struct ye_component_button *button = entity->button;

    //     // if the button is inactive, skip it
    //     if(!button->active)
    //         continue;

    //     // get the position of the button
    //     struct ye_rectf pos = ye_get_position(entity, YE_COMPONENT_BUTTON);

    //     // printf("button %s at %f, %f, %f, %f\n", entity->name, pos.x, pos.y, pos.w, pos.h);

    //     // check if the mouse is hovering over the button
    //     if(
    //         mouseX >= pos.x && mouseX <= pos.x + pos.w &&   // within width
    //         mouseY >= pos.y && mouseY <= pos.y + pos.h      // within height
    //     )
    //     {
    //         button->is_hovered = true;
    //         printf("Hovered\n");
    //         if(event.type == SDL_MOUSEBUTTONDOWN)
    //             button->_clicking = true;
    //         else
    //             button->_clicking = false;

    //         if(button->_clicking && event.type == SDL_MOUSEBUTTONUP){
    //             button->is_clicked = true;
    //             printf("Clicked\n");
    //         }
    //     }
    //     else{
    //         button->is_hovered = false;
    //         button->is_clicked = false;
    //     }

    //     itr = itr->next;
    // }
}

/*
    API FOR ACCESSING STATE:
*/

bool ye_button_hovered(struct ye_entity *entity){
    if(entity == NULL || entity->button == NULL)
        return false;

    return entity->button->is_hovered;
}

bool ye_button_clicked(struct ye_entity *entity){
    if(entity == NULL || entity->button == NULL)
        return false;

    return entity->button->is_clicked;
}
