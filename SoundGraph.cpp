#include "SoundGraph.h"
#include <assert.h>
/*
SoundGraph will store an array of pixels, and write to it when updates need
to be made. When it needs to display, the pixels will be loaded into an
image using image::create.
http://en.sfml-dev.org/forums/index.php?topic=3543.0
*/
#include <algorithm>

using namespace std;

//Control how the graph appears
#define SAMPLES_PER_HORIZONTAL_PIXEL 2048

//Controls smoothing for the low pass filter on the input data
#define SMOOTHING 10

SoundGraph::SoundGraph(int width, int height,int x, int y )
:	pixels(width,height,sf::Color(255,255,255,100)),
	pixelsHigh(height),
	pixelsWide(width),
	smoothedValue(0)
{
	//Set the position by position the sprite
	graphSprite.setPosition(x,y);
	knownMax = INT32_MIN;
	knownMin = INT32_MAX;

	//Set self as the audio event listener
	AudioListenerRecorder::getAudioListenerRecorder()->addSubscriber(this);
}

void SoundGraph::update()
{
	audioQueueMutex.lock();
	//We'll put a dot on the graph on the very right and scroll it left
	int numSamples = unHandledAudio.size();

	//If we have enough samples to draw a pixel, update the graph
	if (numSamples > SAMPLES_PER_HORIZONTAL_PIXEL) {
		//How many pixels should be drawn? For example, if we get 256 samples
		//and do 64 samples per pixel, we should draw 256/64 pixels in this single update.
		int numPixels = numSamples / SAMPLES_PER_HORIZONTAL_PIXEL;

		/*
		Go through the unhandled (unDrawn) audio in one pass. In that, we'll do a few things.
		1. Calculate the current min and max sample values.
		2. Average each group of SAMPLES_PER_HORIZONTAL_PIXEL samples into a single pixel value
		*/

		//In the outer loop, we loop through the number of pixels we have yet to draw
		for (int pixelIndex = 0; pixelIndex < numPixels; pixelIndex++) {
			//In the inner loop, we loop through SAMPLES_PER_HORIZONTAL_PIXEL pixels,
			//update max and min, and come up with an average. We apply a simple LPF to
			//the input before using any values, though
			float total = 0;
			for (int sampleIndex = 0; sampleIndex < SAMPLES_PER_HORIZONTAL_PIXEL; sampleIndex++) {
				//Make everything positive and apply a low pass filter
				float nextSample = abs(unHandledAudio.front());
				assert (!unHandledAudio.empty());
				total += nextSample;
				unHandledAudio.pop_front();
			}
			float average = total / SAMPLES_PER_HORIZONTAL_PIXEL;
			smoothedValue += (average - smoothedValue) / SMOOTHING;
			//Update our known max and min from the average instead of the actual sample values
			knownMax = max<float>(smoothedValue ,knownMax);
			knownMin = min<float>(smoothedValue ,knownMin);
			//We now have an average value to output to the graph for this pixel.
			//Shift the pixel grid left and put this as the rightmost pixel
			pixels.shiftPixels(-1);
			//Calculate the y value of this pixel by scaling
			int y = pixelsHigh - (int) (((smoothedValue - knownMin)/ (knownMax - knownMin)) * pixelsHigh);
			pixels.setPixel(pixelsWide-1,y,sf::Color(122,122,122,100));
		}
	}
	audioQueueMutex.unlock();
}


void SoundGraph::draw(sf::RenderWindow &toRenderOn)
{
	this->graphTexture.loadFromImage(*pixels.getImage());
	this->graphSprite.setTexture(graphTexture);

	toRenderOn.draw(graphSprite);
}

void SoundGraph::processInput( const float* input, int numSamples )
{
	audioQueueMutex.lock();
	//Add the samples to the unprocessed audio queue (to be displayed)
	const float * cur = input;
	for (int i = 0; i < numSamples; i++) {
		unHandledAudio.push_back(cur[i]);
	}	
	audioQueueMutex.unlock();
}

