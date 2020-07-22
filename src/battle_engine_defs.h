int combat_area[2][3];              // character cards in play
int combat_area_card_type[2][3];    // card types (cards in play)
int combat_area_card_hp[2][3];      // card hp (cards in play)
int combat_ready[2][3];             // card combat readiness
int freeze_status[2][3];            // card freeze status
int attack_status[2][3];            // card attack status
int counter_status[2][3];	    // card counter-attack status
int card_effects[2][3];		    // card effect status
int backup_status[2][3];            // backup status
int combat_area_backups[2][3][3];   // backups of combat area characters
int num_backups[2][3];              // number of backups supporting a combat area
                                    // character
                                    
int combat_decks[2][50];            // player decks
int card_types[2][50];              // card type (cards in player decks)
int hand_cards[2][50];              // player cards in hand
int hand_card_type[2][50];          // player cards in hand (type)
int discard_pile_cards[2][50];      // player cards in discard pile
int discard_pile_card_type[2][50];  // player cards in discard pile (type)
int hand_size[2];		    // player hand size
int deck_ptr[2];		    // deck pointer
int discard_pile_ptr[2];            // discard pile pointer

int deploy_status[2];    // flags to determine whether a player
                                // has already deployed a character

int computer_block_pos[3];     // Computer A.I. Block Moves
int human_block_pos[3];        // Human Block Moves      

int player_hp[2];           // player HP
int player_sp[2];           // player SP

int attack_disabled[2];		// player unable to attack
int draw_disabled[2];		// player unable to draw cards

int ac_cost_modifier[2];	// Action card costs modifier
int ac_cost_discounts[2];	// Action card costs discounts

const int BOTH_PLAYERS = 2;
const int COMPUTER_PLAYER = 1;
const int HUMAN_PLAYER = 0;
const int NUM_CARDS = 50;
const int MAX_HAND_SIZE = 50;

const int SNK_CARD = 0;
const int CAPCOM_CARD = 1;
const int ACTION_CARD = 2;

int card_id[2][3];		// Card identification
int card_id_type[2][3];		// For effects that transform cards into another card

int card_ai_value[3][3][200];	// Card AI value
				// Three Types
				// Deployment AI Value
				// Combat AI Value
				// Abilities AI Value

int AI_WEIGHT_FACTORS[3];	// Used in computing overall card value

const int AI_DEPLOY = 0;
const int AI_COMBAT = 1;
const int AI_ABILITY = 2;

int ai_scale_factor = 0;
int triangle_abilities_disabled[2];
int square_abilities_disabled[2];
int circle_abilities_disabled[2];
int action_cards_disabled[2];

int computer_backup = 0;
int computer_backup_card = 0;
int computer_deployed_card = 0;
int player_deployed_card = 0;

const int DECK_PILE_TYPE = 0;
const int DISCARD_PILE_TYPE  = 1;

const int NUM_GAME_CHARS = 120;

void count_hand(int player)
{
    hand_size[player] = 0;
    
    for (int i=0; i<MAX_HAND_SIZE; i++)
    {
        if (hand_cards[player][i]!=255) hand_size[player]++;
    }
}

int count_cards_in_hand(int player, int card_type)
{
    int count = 0;
    
    for (int i=0; i<MAX_HAND_SIZE; i++)
    {
        if (hand_cards[player][i]!=255&&hand_card_type[player][i]==card_type) count++;
    }
    
    return count;
}

int count_cards_in_deck(int player, int card_type)
{
    int count = 0;
    
    for (int i=deck_ptr[player]; i<NUM_CARDS; i++)
    {
        if (card_types[player][i]==card_type) count++;
    }
    
    return count;
}

int count_cards_in_discard_pile(int player, int card_type)
{
    int count = 0;
    
    for (int i=0;i<discard_pile_ptr[player]; i++)
    {
        if (discard_pile_card_type[player][i]==card_type) count++;
    }
    
    return count;
}

int find_first(int player, int card_type, int pile_type)
{
	int return_flag = 0;
	
	if (pile_type == DECK_PILE_TYPE)
	{
		for (int i=deck_ptr[player];i<NUM_CARDS;i++)
		{
			if (card_types[player][i]==card_type&&!return_flag) return_flag = i+1;
		}		
	}
	else
	{
		for (int i=0;i<discard_pile_ptr[player];i++)
		{
			if (discard_pile_card_type[player][i]==card_type&&!return_flag) return_flag = i+1;
		}		
		
	}
	
	return return_flag;
}

int find_first_in_hand(int player, int card_type)
{
	int return_flag = 0;
	
	for (int i=0;i<MAX_HAND_SIZE;i++)
	{
		if (hand_cards[player][i]!=255&&hand_card_type[player][i]&&!return_flag) return_flag = i+1;
	}		
	
	return return_flag;
}

int return_card(int player,int card_num)
{
    int found_card = 0;
    int card_in_hand = 0;
    int i = 0;
    
    while (i<MAX_HAND_SIZE)
    {
        if (hand_cards[player][i]!=255)
        {
                found_card++;
        }
        
        if (found_card == card_num) { card_in_hand = i; break; }
        i++;
    }

    return card_in_hand;
}

int return_char_in_deck(int player, int card_num)
{
	int found_card = 0;
	int i = 0;
	int start_index = deck_ptr[player];
	int card_in_deck = 0;
	
	while ((start_index+i)<NUM_CARDS)
	{
		if (card_types[player][start_index+i]!=ACTION_CARD)
		{
			found_card++;
		}
        	
        	if (found_card == card_num) { card_in_deck = i; break; }
		i++;	
	}
	
	return card_in_deck;
}

int get_hp(int player, int pos)
{
    if (combat_area[player][pos]==255) return 0; else return (combat_area_card_hp[player][pos]);
}

int count_chars(int player)
{
	int num_chars = 0;
	
	for (int i=0;i<3; i++)
	{
		if (combat_area[player][i]!=255) num_chars++;
	}
	
	return num_chars;
}

int count_other_chars(int player, int card_pos)
{
	int num_chars = 0;
	
	for (int j=0; j<2; j++)
	{
		for (int i=0;i<3; i++)
		{
			if (!(player==j&&i==card_pos)&&combat_area[j][i]!=255) num_chars++;
		}
	}
	
	return num_chars;
}

int united_invincible_Capcom(int player, int block_pos)
{
	int return_flag = 0;
	
	// Makoto: Yell
	if (combat_area[player][block_pos] == 42) return_flag = 1;

	return return_flag;
}

int united_invincible_SNK(int player, int block_pos)
{
	int return_flag = 0;
	
	// Shigen: Steel Bulb
	if (combat_area[player][block_pos] == 99) return_flag = 1;

	return return_flag;
}

int united_invincible(int player, int block_pos)
{
	int return_flag = 0;
	
	if (circle_abilities_disabled[player]) return 0;
	
	if (combat_area_card_type[player][block_pos]==SNK_CARD) return_flag = united_invincible_SNK(player,block_pos);
	if (combat_area_card_type[player][block_pos]==CAPCOM_CARD) return_flag = united_invincible_Capcom(player,block_pos);
	
	return return_flag;
}

int check_if_snk_blockable(int player, int attacker_pos)
{
	int return_flag = 1;
	return return_flag;	
}

int check_if_capcom_blockable(int player, int attacker_pos)
{
	int opponent = 1 - player;
	int return_flag = 1;
	
	if (combat_area[player][attacker_pos] == 20)
	{
		// Guy: Haya-Gake
		// Cannot be blocked if enemy has less than 2 characters
		if (count_chars(opponent)<2) return_flag = 0;
	}

	if (combat_area[player][attacker_pos] == 116)
	{
		// Nina: Wings
		// Cannot be blocked if enemy has no cards in hand
		count_hand(opponent);
		if (!hand_size[opponent]) return_flag = 0;
	}

	return return_flag;	
}

int check_if_blockable(int player, int attacker_pos)
{
	int return_flag = 1;
	
	if (circle_abilities_disabled[player]) return 1;
	if (combat_area[player][attacker_pos]==255) return 1;
	
	if (combat_area_card_type[player][attacker_pos] == CAPCOM_CARD) return_flag = check_if_capcom_blockable(player,attacker_pos);
	if (combat_area_card_type[player][attacker_pos] == SNK_CARD) return_flag = check_if_snk_blockable(player,attacker_pos);
	
	return return_flag;
}


int check_xcopy_SNK(int player, int block_pos)
{
	int return_flag = 0;
	return return_flag;	
}

int check_xcopy_Capcom(int player, int block_pos)
{
	int return_flag = 0;
	
	// Tweleve: XCOPY
	// When blocking, BP becomes BP of attacker
	if (combat_area[player][block_pos]==44) return_flag = 1;
	
	return return_flag;	
}

int check_xcopy(int player, int block_pos)
{
	int return_flag = 0;
	
	if (circle_abilities_disabled[player]) return 0;
	if (combat_area_card_type[player][block_pos]==SNK_CARD) return_flag = check_xcopy_SNK(player,block_pos);
	if (combat_area_card_type[player][block_pos]==CAPCOM_CARD) return_flag = check_xcopy_Capcom(player,block_pos);
	
	return return_flag;
}

int check_card_ai_value(int player,int card_num, int ai_type)
{
	// checks the AI value of the card.
	// there are three AI values for a card:
	// Deployment, Combat, Ability
	// the computer decides whether to
	// deploy a card based on these
	// set of values.
	// Deployment - Value is high if deployment of this card maximizes computer advantage
	// Combat - Value is high if it is a good attacker/blocker
	// Ability - Value is high if it has a useful ability

	int card = return_card(player,card_num);
	return card_ai_value[hand_card_type[player][card]][ai_type][hand_cards[player][card]];
}

int check_card_in_deck_ai_value(int deck_owner, int card_num, int ai_type)
{
	// checks the AI value of the card (in deck).
	// there are three AI values for a card:
	// Deployment, Combat, Ability
	// the computer decides whether to
	// deploy a card based on these
	// set of values.
	// Deployment - Value is high if deployment of this card maximizes computer advantage
	// Combat - Value is high if it is a good attacker/blocker
	// Ability - Value is high if it has a useful ability
	return card_ai_value[card_types[deck_owner][card_num]][ai_type][combat_decks[deck_owner][card_num]];
}

int check_card_in_discards_ai_value(int deck_owner, int card_num, int ai_type)
{
	// checks the AI value of the card (in discards).
	// there are three AI values for a card:
	// Deployment, Combat, Ability
	// the computer decides whether to
	// deploy a card based on these
	// set of values.
	// Deployment - Value is high if deployment of this card maximizes computer advantage
	// Combat - Value is high if it is a good attacker/blocker
	// Ability - Value is high if it has a useful ability
	return card_ai_value[discard_pile_card_type[deck_owner][card_num]][ai_type][discard_pile_cards[deck_owner][card_num]];
}


int check_char_ai_value(int card_pos, int ai_type)
{
	// checks the AI value of the card (in play).
	// there are three AI values for a card:
	// Deployment, Combat, Ability
	// the computer decides whether to
	// deploy a card based on these
	// set of values.
	// Deployment - Value is high if deployment of this card maximizes computer advantage
	// Combat - Value is high if it is a good attacker/blocker
	// Ability - Value is high if it has a useful ability
	return card_ai_value[combat_area_card_type[COMPUTER_PLAYER][card_pos]][ai_type][combat_area[COMPUTER_PLAYER][card_pos]];
}
