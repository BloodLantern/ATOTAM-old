#ifndef _PENNER_BACK
#define _PENNER_BACK

class Back {
	
	public:
		// Time, Base value, Change (difference between ending and starting value), Duration (total)
		static float easeIn(float t,float b , float c, float d);
		static float easeOut(float t,float b , float c, float d);
		static float easeInOut(float t,float b , float c, float d);
		
};

#endif