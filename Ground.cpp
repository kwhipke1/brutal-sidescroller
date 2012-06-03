#include "Ground.h"
#include "image_util.h"
#include "TextureManager.h"


//Initialize static members
sf::Texture Ground::groundTexture;
bool Ground::loaded = false;

Ground::Ground(float xPos, float yPos, float width, float height, int scrollSpeed)
	: 
	GameObject(),
	scrollSpeed(scrollSpeed)
{
	//Check if the texture for ground has already
	//been created. If not, create it.
	if (!loaded)
	{
		Ground::groundTexture = *(gTextureManager.getResource("ground.png"));
		//For looping
		Ground::groundTexture.setRepeated(true);
	}

	//make the sprite use the ground texture, put it in position
	sprite.setTexture(Ground::groundTexture);
	sprite.setTextureRect(sf::IntRect(0,0,width,height));
	sprite.setPosition(xPos,yPos);	
}


Ground::~Ground(void)
{
}

sf::Sprite& Ground::getSprite()
{
	return sprite;
}

void Ground::update()
{
	//Advance the scrolling
	int curX = sprite.getTextureRect().left;
	float nextX = curX + scrollSpeed % sprite.getTextureRect().width;
	sprite.setTextureRect(sf::IntRect(nextX,sprite.getTextureRect().top,sprite.getTextureRect().width,sprite.getTextureRect().height));
}