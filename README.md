# ambisonic-toolbox-JUCE
Collection of ambisonic plug-ins and apps developed with JUCE.

**quad-bin-dec:** uses speakers LF, LB, RB, FR along with the LISTEN (IRCAM) dataset.

All of these use ambix.

I developped these at UCSD during my studies under Tom Erbe. This was just an introduction to the JUCE framework. Unfortunately I am not sure how the convolution routine is implemented by JUCE which led to a lot of difficulty. Eventually I managed to get clear sound but I couldn't notice a discernible difference when rotating the soundfield with or without the convolution which is dissapointing. 

I am hoping to return to this project and implement my own convolution. I also want to improve the GUI by making use of the .obj rendering capabilities that JUCE has, however, I assume this will cost users performance. Perhaps a low-res .OBJ file could provide information w/o being too demanding. 
