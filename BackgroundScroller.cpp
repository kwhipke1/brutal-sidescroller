#include "BackgroundScroller.h"
#include "image_util.h"
#include "TextureManager.h"


bool BackgroundScroller::loaded = false;

BackgroundScroller::BackgroundScroller(const std::string pathToImageInSpriteFolder, float xPos, float yPos, float width, float height, int scrollSpeed)
	: 
GameObject(),
	scrollSpeed(scrollSpeed),
	ticksSinceScroll(0)
{
	//Check if the texture for ground has already
	//been created. If not, create it.
	if (!loaded)
	{
		groundTexture = *(gTextureManager.getResource(pathToImageInSpriteFolder));
		//For looping
		groundTexture.setRepeated(true);
	}

	//make the sprite use the ground texture, put it in position
	sprite.setTexture(BackgroundScroller::groundTexture);
	sprite.setTextureRect(sf::IntRect(0,0,width,height));
	sprite.setPosition(xPos,yPos);	
}


BackgroundScroller::~BackgroundScroller(void)
{
}

void BackgroundScroller::draw(sf::RenderWindow &toRenderOn)
{
	toRenderOn.draw(sprite);
}

void BackgroundScroller::update()
{
	//Advance the scrolling
	ticksSinceScroll += 1;

	if (ticksSinceScroll > scrollSpeed) {
		int curX = sprite.getTextureRect().left;
		float nextX = curX + 1 % sprite.getTextureRect().width;
		sprite.setTextureRect(sf::IntRect(nextX,sprite.getTextureRect().top,sprite.getTextureRect().width,sprite.getTextureRect().height));
		ticksSinceScroll = 0;
	}
}
