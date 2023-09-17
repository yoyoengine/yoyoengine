/*
    This is here for reference when I integrate interactions into the ECS
*/

// // linked list holding pointers towards button render objects
// typedef struct button {
//     bool isObject;
//     struct renderObject *pObject;
//     struct button *pNext;
//     struct callbackData *callbackData;
// } button;

// struct callbackData {
//     char *callbackType;
//     void (*callback)(struct callbackData *data);
//     void *pData;
// };

// void removeButton(int id){
//     // step through button LL and find which node has the ID of the renderobject passed
//     // then, we can remove that node from the LL and free it and call removeRenderObject() on the renderObject
//     if(pButtonListHead == NULL){
//         // logMessage(warning, "ERROR REMOVING BUTTON: HEAD IS NULL\n");
//     }
//     else{
//         button *pCurrent = pButtonListHead;
//         while(pCurrent->pNext != NULL && pCurrent->pNext->pObject->identifier != id){
//             pCurrent = pCurrent->pNext;
//         }
//         if(pCurrent->pNext != NULL){
//             button *pToDelete = pCurrent->pNext;
//             pCurrent->pNext = pToDelete->pNext;

//             // remove callback data (game hadnles its own callbackData)
//             free(pToDelete->callbackData->callbackType);
//             free(pToDelete->callbackData);

//             free(pToDelete);
//             removeRenderObject(id);
//         }
//         else{
//             char buffer[100];
//             snprintf(buffer, sizeof(buffer),  "ERROR: COULD NOT FIND BUTTON WITH ID#%d TO DELETE\n",id);
//             logMessage(error, buffer);
//         }
//     }
// }

// // helper function for clearAll() to remove all buttons from the button LL and their render objects
// void clearAllButtons(){
//     if(pButtonListHead == NULL){
//         // logMessage(warning, "ERROR REMOVING ALL BUTTONS: HEAD IS NULL\n");
//     }
//     else{
//         button *pCurrent = pButtonListHead;
//         while(pCurrent != NULL){
//             button *pToDelete = pCurrent;
//             pCurrent = pCurrent->pNext;

//             // char buffer[100];
//             // snprintf(buffer, sizeof(buffer),  "Remove button object id#%d\n", pToDelete->pObject->identifier);
//             // logMessage(debug, buffer);
//             removeRenderObject(pToDelete->pObject->identifier);
            
//             // we decref the json_t object inside the callbackData struct, because its a new ref so it will be freed
//             // json_decref(pToDelete->callbackData->pJson); TODO: handle callback data leaks

//             // we free our dynamically allocated fields
//             free(pToDelete->callbackData->callbackType); // free the malloced type string
//             free(pToDelete->callbackData); // free the malloced callbackData struct
            
//             free(pToDelete); // free button object
//         }
//         pButtonListHead = NULL;
//     }
// }

// /*
//     method to create an engine button
//     Takes in a string path to the background, font, text color, relative x, relative y, relative width, relative height
//     CONSIDERATIONS / TODO: 
//     - formatting the text such that it can be passed left, center, or right aligned and does not stretch to fill 
//     - refactor texture rendering to external function so button textures can be generated and replaced externally in the future, for now buttons are static (maybe that texture can be auto modified by pointer in struct)
//     - eventually we need to support outlined text in buttons, think about the best way to accomplish that
// */
// int createButton(int depth, float x, float y, float width, float height, char *pText, TTF_Font *pFont, SDL_Color *pColor, bool centered, char *pBackgroundPath, struct callbackData *data, Alignment alignment) {
//     // get our real coordinate bounds for the object
//     SDL_Rect bounds = createRealPixelRect(centered, x, y, width, height);

//     SDL_Texture *textTexture = createTextTexture(pText, pFont, pColor);

//     if (textTexture == NULL) {
//         logMessage(error, "ERROR CREATING TEXT TEXTURE FOR BUTTON\n");
//         return intFail;
//     }

//     struct textureInfo info = createImageTexture(pBackgroundPath,true);
//     SDL_Texture *pImageTexture = info.pTexture;

//     if(pImageTexture == NULL){
//         logMessage(error, "ERROR CREATING IMAGE TEXTURE FOR BUTTON\n");
//         SDL_DestroyTexture(textTexture);
//         return intFail;
//     }

//     int imgWidth, imgHeight;
//     SDL_QueryTexture(pImageTexture, NULL, NULL, &imgWidth, &imgHeight);
//     SDL_Rect imgRect = {0,0,imgWidth,imgHeight};
//     autoFitBounds(&bounds, &imgRect, alignment);

//     SDL_Texture* buttonTexture = SDL_CreateTexture(pRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, imgWidth, imgHeight);

//     if (buttonTexture == NULL) {
//         logMessage(error, "ERROR CREATING BUTTON TEXTURE\n");
//         SDL_DestroyTexture(textTexture);
//         return intFail;
//     }

//     // Set the new texture as the render target
//     SDL_SetRenderTarget(pRenderer, buttonTexture);

//     // Render the background image onto the new texture
//     SDL_Rect backgroundRect = {0, 0, imgWidth, imgHeight};
//     SDL_RenderCopy(pRenderer, pImageTexture, NULL, &backgroundRect);

//     // Get dimensions of the text texture
//     int textWidth, textHeight;
//     SDL_QueryTexture(textTexture, NULL, NULL, &textWidth, &textHeight);

//     // just text alignement...? ///////////////////////////////////////////

//     float widthRatio = (float)imgWidth / textWidth;
//     float heightRatio = (float)imgHeight / textHeight;
//     float scale = fminf(widthRatio, heightRatio);

//     int scaledTextWidth = (int)(textWidth * scale);
//     int scaledTextHeight = (int)(textHeight * scale);

//     // Calculate the position of the text to center it within the button
//     SDL_Rect textRect;
//     textRect.x = (imgWidth - scaledTextWidth) / 2;
//     textRect.y = (imgHeight - scaledTextHeight) / 2;
//     textRect.w = scaledTextWidth;
//     textRect.h = scaledTextHeight;

//     ///////////////////////////////////////////////////////////////////////

//     // Render the text onto the new texture
//     SDL_RenderCopy(pRenderer, textTexture, NULL, &textRect);

//     // Reset the render target to the default
//     SDL_SetRenderTarget(pRenderer, NULL);

//     global_id++; // to stay consistant, increment now and refer to global_id - 1 when accessing ID

//     renderObject staging = {
//         global_id - 1,
//         depth,
//         renderType_Button,
//         buttonTexture,
//         imgRect,
//         NULL,
//         false, // total baked button texture should never be cached
//         bounds,
//         centered,
//         alignment,
//         {
//             .ButtonData = {
//                 .TextData = {
//                     .pFont = pFont,
//                     .outlineSize = 0,
//                     .pColor = pColor,
//                     .pOutlineColor = NULL,
//                     .pText = pText
//                 },
//                 .ImageData = {
//                     .pPath = pBackgroundPath,
//                 }
//             }
//         }
//     };

//     addRenderObject(staging);

//     renderObject *pObj = getRenderObject(global_id - 1);

//     // construct and malloc new button
//     button *pButton = (button *)malloc(sizeof(button));
//     pButton->pObject = pObj;
//     pButton->pNext = NULL;
//     pButton->callbackData = data;

//     // Add the new button to the linked list
//     // (sorted by depth, highest at head)
//     if(pButtonListHead == NULL){
//         pButtonListHead = pButton;
//     }
//     else{
//         button *pCurrent = pButtonListHead;
//         while(pCurrent->pNext != NULL && pCurrent->pNext->pObject->depth < pButton->pObject->depth){
//             pCurrent = pCurrent->pNext;
//         }
//         pButton->pNext = pCurrent->pNext;
//         pCurrent->pNext = pButton;
//     }

//     // Cleanup
//     SDL_DestroyTexture(textTexture);

//     return global_id - 1; // for consistancy
// }

// // function that traverses our LL of buttons and returns the highest depth
// // button clicked by ID, NULL if none
// void checkClicked(int x, int y){
//     // create a temp button pointer to increment the list
//     button *pCurrent = pButtonListHead;

//     // while the next struct is not null
//     while (pCurrent != NULL) {
//         // check if we have clicked inside the button
//         if (x >= pCurrent->pObject->rect.x +xOffset &&
//             x <= pCurrent->pObject->rect.x + pCurrent->pObject->rect.w +xOffset &&
//             y >= pCurrent->pObject->rect.y +yOffset &&
//             y <= pCurrent->pObject->rect.y + pCurrent->pObject->rect.h + yOffset) 
//         {
//             // run the buttons callback if its not null
//             if(pCurrent->callbackData->callback != NULL && pCurrent->callbackData != NULL && pCurrent->callbackData->pData != NULL){
//                 logMessage(debug, "Button clicked, running callback\n");
//                 pCurrent->callbackData->callback(pCurrent->callbackData);
//                 logMessage(debug, "Callback finished\n");
//                 return;
//             }
//             else{
//                 logMessage(warning, "ERROR: CLICKED BUTTON CALLBACK IS NULL\n");
//                 return;
//             }
//             // return pCurrent->pObject->identifier; // return our current
//         }
//         // else increment
//         pCurrent = pCurrent->pNext;
//     }
//     // if no object exists with identifier, return NULL
//     //return intFail;
// }
