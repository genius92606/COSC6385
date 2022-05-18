// my_predictor.h
// This file contains a sample gshare_predictor class.
// It is a simple 32,768-entry gshare with a history length of 15.

class gshare_update : public branch_update {
public:
	unsigned int index;
};

class gshare_predictor : public branch_predictor {
public:
#define HISTORY_LENGTH	15
#define TABLE_BITS	15
	gshare_update u;
	branch_info bi;
	unsigned int history;
	unsigned char tab[1<<TABLE_BITS];

	gshare_predictor (void) : history(0) { 
		memset (tab, 0, sizeof (tab));
	}

	branch_update *predict (branch_info & b) {
		bi = b;
		if (b.br_flags & BR_CONDITIONAL) {
			u.index = 
				  (history << (TABLE_BITS - HISTORY_LENGTH)) 
				^ (b.address & ((1<<TABLE_BITS)-1));

			u.direction_prediction (tab[u.index] >> 1);
		} else {
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
	}

	void update (branch_update *u, bool taken, unsigned int target) {
		if (bi.br_flags & BR_CONDITIONAL) {
			unsigned char *c = &tab[((gshare_update*)u)->index];
			if (taken) {
				if (*c < 3) (*c)++;
			} else {
				if (*c > 0) (*c)--;
			}
			history <<= 1;
			history |= taken;
			history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};



//
// Pentium M hybrid branch predictors
// This class implements a simple hybrid branch predictor based on the Pentium M branch outcome prediction units. 
// Instead of implementing the complete Pentium M branch outcome predictors, the class below implements a hybrid 
// predictor that combines a bimodal predictor and a global predictor. 
class pm_update : public branch_update {
public:
        unsigned int index; 
        unsigned int g_index;
};

class pm_predictor : public branch_predictor {
public:
#define TABLE_BITS	15
#define HISTORY_LENGTH	15

        pm_update u;
	branch_info bi;
	unsigned int history;
	unsigned char g_tab[1<<TABLE_BITS];
	unsigned char b_tab[1<<TABLE_BITS];
        pm_predictor (void) : history(0) {
        	memset (g_tab, 0, sizeof (g_tab));
        	memset (b_tab, 0, sizeof (b_tab));
        }
        branch_update *predict (branch_info & b) {
        	bi=b;
        	
		if (b.br_flags & BR_CONDITIONAL) { 
			u.index = b.address & ((1<<TABLE_BITS)-1); // address & all 1 bits
			u.g_index= (history << (TABLE_BITS - HISTORY_LENGTH)) ^ u.index;

			if(history>>1<<1) //show right most bit
				u.direction_prediction(g_tab[u.g_index]>>1);	//use global
			else
				u.direction_prediction(b_tab[u.index]>>1);	//use bimodal

		} else { 
			u.direction_prediction (true);
		}
		u.target_prediction (0);
		return &u;
        }

        void update (branch_update *u, bool taken, unsigned int target) {

        	if (bi.br_flags & BR_CONDITIONAL) {
        		unsigned char *b = &b_tab[((pm_update*)u)->index];
        		unsigned char *g = &g_tab[((pm_update*)u)->g_index];

        		if (taken) {		//2-bit counter
				if (*g < 3) (*g)++; if (*b < 3) (*b)++;
					
			} else {
				if (*g > 0) (*g)--; if (*b > 0) (*b)--;
			}
        		history <<= 1;
			history |= taken;
			history &= (1<<HISTORY_LENGTH)-1;
		}
	}
};


//
// Complete Pentium M branch predictors for extra credit
// This class implements the complete Pentium M branch prediction units. 
// It implements both branch target prediction and branch outcome predicton. 
class cpm_update : public branch_update {
public:
        unsigned int index;
};

class cpm_predictor : public branch_predictor {
public:
        cpm_update u;

        cpm_predictor (void) {
        }

        branch_update *predict (branch_info & b) {
            u.direction_prediction (true);
            u.target_prediction (0);
            return &u;
        }

        void update (branch_update *u, bool taken, unsigned int target) {
        }

};


