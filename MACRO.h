#define OM_S_Cr	1		//opponent modeling, showdown coefficient of number of raises in this game.
#define OM_S_Cc 1		//
#define OM_NS_MAX 5		//no showdown max opponent tightness
#define OM_alpha 0.5	//combining the above two to get final opponent tightness
#define OM_rounds 100	//number of rounds remembered

///////////////////////////

#define DS_DP 3		//decision making, dealer penalty pre-flop
#define DS_SP 2		//
#define DS_BP 0		//
#define DS_Cr 1		//coefficient of number of raises in current round
#define DS_Cc 1		//coefficient of number of calls in current round
#define DS_Cundo 1	//penalty for opponents that did not act yet
#define DS_THr 1	//Threshold between raise and call
#define DS_THc 1	//Threshold between call and fold
