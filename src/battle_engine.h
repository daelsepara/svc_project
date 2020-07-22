void swap_cards(int player, int pos_1, int pos_2)
{
	int temp = 0;
	
	if (!pos_1||!pos_2) return;
	if (pos_1 == pos_2) return;
	                                    	      
	pos_1 --;
	pos_2 --;
	                                    	      
	temp = combat_area[player][pos_1];
	combat_area[player][pos_1] = combat_area[player][pos_2];
	combat_area[player][pos_2] = temp;

	temp = combat_area_card_type[player][pos_1];
	combat_area_card_type[player][pos_1] = combat_area_card_type[player][pos_2];
	combat_area_card_type[player][pos_2] = temp;

	temp = combat_area_card_hp[player][pos_1];
	combat_area_card_hp[player][pos_1] = combat_area_card_hp[player][pos_2];
	combat_area_card_hp[player][pos_2] = temp;

	temp = combat_ready[player][pos_1];
	combat_ready[player][pos_1] = combat_ready[player][pos_2];
	combat_ready[player][pos_2] = temp;

	temp = freeze_status[player][pos_1];
	freeze_status[player][pos_1] = freeze_status[player][pos_2];
	freeze_status[player][pos_2] = temp;

	temp = attack_status[player][pos_1];
	attack_status[player][pos_1] = attack_status[player][pos_2];
	attack_status[player][pos_2] = temp;

	temp = counter_status[player][pos_1];
	counter_status[player][pos_1] = counter_status[player][pos_2];
	counter_status[player][pos_2] = temp;

	temp = backup_status[player][pos_1];
	backup_status[player][pos_1] = backup_status[player][pos_2];
	backup_status[player][pos_2] = temp;

	temp = num_backups[player][pos_1];
	num_backups[player][pos_1] = num_backups[player][pos_2];
	num_backups[player][pos_2] = temp;

    	temp = card_effects[player][pos_1];
	card_effects[player][pos_1] = card_effects[player][pos_2];
	card_effects[player][pos_2] = temp;

    	temp = card_id[player][pos_1];
	card_id[player][pos_1] = card_id[player][pos_2];
	card_id[player][pos_2] = temp;

    	temp = card_id_type[player][pos_1];
	card_id_type[player][pos_1] = card_id_type[player][pos_2];
	card_id_type[player][pos_2] = temp;
	
	for (int i=0; i<3; i++)
	{
		temp = combat_area_backups[player][pos_1][i];
		combat_area_backups[player][pos_1][i] = combat_area_backups[player][pos_2][i];
		combat_area_backups[player][pos_2][i] = temp;
	}
}

void init_ai(void)
{
	for (int j=0;j<NUM_GAME_CHARS;j++)
	{
		for (int i=0;i<3;i++)
		{
			card_ai_value[SNK_CARD][i][j] = SNK_AI_VALUES[j*3+i];
			card_ai_value[CAPCOM_CARD][i][j] = CAPCOM_AI_VALUES[j*3+i];
		}
	}
	
	for (int j=120; j<200; j++)
	{
		card_ai_value[ACTION_CARD][0][j] = 0;
		card_ai_value[ACTION_CARD][1][j] = 0;
		card_ai_value[ACTION_CARD][2][j] = 0;
	}

	for (int j=120; j<200; j++)
	{
		for (int i=0;i<3;i++)
		{
			card_ai_value[SNK_CARD][i][j] = 0;
			card_ai_value[CAPCOM_CARD][i][j] = 0;
		}
	}
}

void init_discard_pile(int player)
{
    for (int i=0;i<NUM_CARDS;i++)
    {
        discard_pile_cards[player][i] = 255;
        discard_pile_card_type[player][i] = 255;
    }
    
    discard_pile_ptr[player] = 0;
}

void build_random_deck(int player)
{
    	for (int i=0;i<NUM_CARDS;i++)
    	{
        	int card = (int)(rand()%240);
        	
        	card_types[player][i] = card/120;
        	
        	if (card>119) card -= 120;

        	combat_decks[player][i] = card;
        	
        	hand_cards[player][i] = 255;
        	hand_card_type[player][i] = 255;
	}
    	
    deck_ptr[player] = 0;
}

void put_to_discard_pile(int player,int card_pos)
{
    discard_pile_cards[player][discard_pile_ptr[player]] = combat_area[player][card_pos];
    discard_pile_card_type[player][discard_pile_ptr[player]] = combat_area_card_type[player][card_pos];
    discard_pile_ptr[player]++;  
}

void kill_character_effects_SNK(int player, int card_pos)
{
	int opponent = 1 - player;
	if (combat_area[player][card_pos] == 46)
	{
		// Nicotine killed. Enable All square abilities
		square_abilities_disabled[player] = 0;
		square_abilities_disabled[opponent] = 0;
	}

	if (combat_area[player][card_pos] == 53)
	{
		// Shiki (T) killed. Reduce AC card costs
		for (int i=0; i<2; i++)
		{
			ac_cost_modifier[i] -= 3;
			if (ac_cost_modifier[i]<0) ac_cost_modifier[i] = 0;
		}
	}
}

void kill_character_effects_CAPCOM(int player, int card_pos)
{
	int opponent = 1 - player;
	if (combat_area[player][card_pos] == 3)
	{
		// Guile killed. Enable All triangle abilities
		triangle_abilities_disabled[player] = 0;
		triangle_abilities_disabled[opponent] = 0;
	}
	
	if (combat_area[player][card_pos] == 58)
	{
		// Anakaris Killed. Enable all character abilities again
		for (int i=0; i<2; i++)
		{
			circle_abilities_disabled[i] = 0;
			square_abilities_disabled[i] = 0;
		}
	}

	if (combat_area[player][card_pos] == 34)
	{
		// Alex Killed. Enable the use of action cards
		action_cards_disabled[opponent] = 0;
	}
	
	if (combat_area[player][card_pos] == 81)
	{
		// Kyosuke killed. Reduce AC card discounts
		for (int i=0; i<2; i++)
		{
			ac_cost_discounts[i] -= 2;
			if (ac_cost_discounts[i]<0) ac_cost_discounts[i] = 0;
		}
	}

}

void kill_character_effects(int player, int card_pos)
{
	if (combat_area[player][card_pos]==card_id[player][card_pos]&&combat_area_card_type[player][card_pos]==card_id_type[player][card_pos])
	{
		if (combat_area_card_type[player][card_pos] == SNK_CARD) kill_character_effects_SNK(player,card_pos);
		if (combat_area_card_type[player][card_pos] == CAPCOM_CARD) kill_character_effects_CAPCOM(player,card_pos);
	}
	else
	{
    		combat_area[player][card_pos] = card_id[player][card_pos];
    		combat_area_card_type[player][card_pos] = card_id_type[player][card_pos];
	}
}

void kill_character(int player, int card_pos)
{
    kill_character_effects(player,card_pos);
    combat_area_card_hp[player][card_pos] = 0;
    combat_area[player][card_pos] = 255;
    combat_area_card_type[player][card_pos] = 255;
}

void draw_effects_snk(int player, int opponent, int card_pos)
{
	
}

void draw_effects_capcom(int player, int opponent, int card_pos)
{
	if (combat_area[opponent][card_pos] == 38)
	{
		// Oro: Tengu Stone
		// Gains 100 for each card drawn by opponent
		combat_area_card_hp[opponent][card_pos]+=100;
	}
}

void draw_effects(int player)
{
	int opponent = 1 - player;
	
	if (circle_abilities_disabled[player]) return;
	
	for (int i=0; i<3; i++)
	{
		if (combat_area[player][i]!=255)
		{
			if (combat_area_card_type[player][i] == SNK_CARD) draw_effects_snk(player,opponent,i);
			if (combat_area_card_type[player][i] == CAPCOM_CARD) draw_effects_capcom(player,opponent,i);
		}
		
		if (combat_area[opponent][i]!=255)
		{
			if (combat_area_card_type[opponent][i] == SNK_CARD) draw_effects_snk(player,opponent,i);
			if (combat_area_card_type[opponent][i] == CAPCOM_CARD) draw_effects_capcom(player,opponent,i);
		}
	}
}

int draw_card(int player)
{
    if (deck_ptr[player] == NUM_CARDS) return 0;
    
    int hand_ptr = 0;
    
    int exit_flag = 0;
    
    while(hand_ptr<MAX_HAND_SIZE&&!exit_flag)
    {
        if (hand_cards[player][hand_ptr] == 255)
        {
                hand_cards[player][hand_ptr] = combat_decks[player][deck_ptr[player]];
                hand_card_type[player][hand_ptr] = card_types[player][deck_ptr[player]];
                exit_flag = 1;
        }
        else
        {
                hand_ptr++;
        }
    }

    deck_ptr[player]++;
    
    draw_effects(player);
    
    return 1;    
}

const int shuffle_passes = 4;
void shuffle_deck(int player)
{
	// shuffles player deck
	int deck_size = (NUM_CARDS - deck_ptr[player]) + 1;
	
	for (int j=0; j < shuffle_passes; j++)
	{
		for (int i=deck_ptr[player];i<NUM_CARDS;i++)
		{
			int temp = 0;
			int card_a = rand()%deck_size;
			int card_b = rand()%deck_size;
		
			temp = combat_decks[player][deck_ptr[player]+card_a];
			combat_decks[player][deck_ptr[player]+card_a] = combat_decks[player][deck_ptr[player]+card_b];
			combat_decks[player][deck_ptr[player]+card_b] = temp;
		
			temp = card_types[player][deck_ptr[player]+card_a];
			card_types[player][deck_ptr[player]+card_a] = card_types[player][deck_ptr[player]+card_b];
			card_types[player][deck_ptr[player]+card_b] = temp;
		}
	}
}

int search_for_card(int player, int card, int card_type, int pile_type)
{
	int return_flag = 0;
	
	if (pile_type == DECK_PILE_TYPE)
	{
		int found = 0;
		for (int i=deck_ptr[player];i<NUM_CARDS;i++)
		{
			if (!found)
			{
				if (combat_decks[player][i]==card&&card_types[player][i]==card_type)
				{
					found = 1;
					return_flag = i+1;
				}
			}
		}
	}
	else
	{
		int found = 0;
		for (int i=0;i<discard_pile_ptr[player];i++)
		{
			if (!found)
			{
				if (discard_pile_cards[player][i]==card&&discard_pile_card_type[player][i]==card_type)
				{
					found = 1;
					return_flag = i+1;
				}
			}
		}
	}
	
	return return_flag;
}

void put_on_top(int player, int card_num)
{
	int temp1 = 0;
	int temp2 = 0;
	
	temp1 = combat_decks[player][card_num];
	temp2 = card_types[player][card_num];
	for (int i=card_num;i>deck_ptr[player]; i--)
	{
		combat_decks[player][i] = combat_decks[player][i-1];
		card_types[player][i] = card_types[player][i-1];
	}
	combat_decks[player][deck_ptr[player]] = temp1;
	card_types[player][deck_ptr[player]] = temp2;
}

void put_to_bottom(int player, int card_num)
{
	int temp1 = 0;
	int temp2 = 0;
	
	temp1 = combat_decks[player][card_num];
	temp2 = card_types[player][card_num];
	
	for (int i=card_num;i<NUM_CARDS-1;i++)
	{
		combat_decks[player][i] = combat_decks[player][i+1];
		card_types[player][i] = card_types[player][i+1];
	}
	
	combat_decks[player][NUM_CARDS-1] = temp1;
	card_types[player][NUM_CARDS-1] = temp2;
}

void put_to_bottom_of_discard(int player, int card_num)
{
	int temp1 = 0;
	int temp2 = 0;
	
	temp1 = discard_pile_cards[player][card_num];
	temp2 = discard_pile_card_type[player][card_num];
	
	for (int i=card_num;i<discard_pile_ptr[player]-1;i++)
	{
		discard_pile_cards[player][i] = discard_pile_cards[player][i+1];
		discard_pile_card_type[player][i] = discard_pile_card_type[player][i+1];
	}
	
	discard_pile_cards[player][discard_pile_ptr[player]-1] = temp1;
	discard_pile_card_type[player][discard_pile_ptr[player]-1] = temp2;
}

void return_to_top_of_deck_from_bottom_of_discard(int player)
{
	if (discard_pile_ptr[player]==0) return;
	
	deck_ptr[player]--;
	combat_decks[player][deck_ptr[player]] = discard_pile_cards[player][discard_pile_ptr[player]-1];
	card_types[player][deck_ptr[player]] = discard_pile_card_type[player][discard_pile_ptr[player]-1];
	discard_pile_ptr[player]--;
}

void return_to_top(int player, int card_pos)
{
	if (deck_ptr[player]==0) return;
	
	deck_ptr[player]--;
	combat_decks[player][deck_ptr[player]] = combat_area[player][card_pos];
	card_types[player][deck_ptr[player]] = combat_area_card_type[player][card_pos];
	combat_area_card_hp[player][card_pos] = 0;
    	combat_area[player][card_pos] = 255;
	combat_area_card_type[player][card_pos] = 255;
}

void return_to_hand(int player, int card_pos)
{
	int found = 0;
	for (int i=0; i<MAX_HAND_SIZE; i++)
	{
		if (!found)
		{
			if (hand_cards[player][i] == 255)
			{
				hand_cards[player][i] = combat_area[player][card_pos];
        			hand_card_type[player][i] = combat_area_card_type[player][card_pos];
				combat_area_card_hp[player][card_pos] = 0;
    				combat_area[player][card_pos] = 255;
				combat_area_card_type[player][card_pos] = 255;
				found = 1;
        		}
        	}
	}
}

int draw_cards(int player, int num_cards)
{
	for (int i=0; i<num_cards; i++) draw_card(player);
	
	return 1;
}

void init_hand(int player)
{
        draw_cards(player,5);
}

void clear_deploy_status(int player)
{
    deploy_status[player] = 0;
}

void clear_backup_status(int player)
{
    for (int i=0;i<3; i++) backup_status[player][i] = 0;
}

void set_deploy_status(int player)
{
    deploy_status[player] = 1;
}

void clear_combat_area(void)
{
    for (int y = 0;y<2;y++)
    {
        for (int x = 0;x<3;x++)
        {
                combat_area[y][x] = 255;
                combat_area_card_type[y][x] = 255;
                combat_area_card_hp[y][x] = 0;
                freeze_status[y][x] = 0;
                attack_status[y][x] = 0;
                backup_status[y][x] = 0;
                counter_status[y][x] = 0;
                num_backups[y][x] = 0;
                card_id[y][x] = 255;
                card_id_type[y][x] = 255;
        }
    }
}

void clear_freeze_status(int player)
{
    for (int x = 0;x<3;x++)
    {
        if (freeze_status[player][x])
        {	
        	freeze_status[player][x]--;
        	if (counter_status[player][x]!=255) counter_status[player][x] = 0;
        }
    }
}

void enable_action_cards(int player)
{
    for (int x = 0;x<3;x++)
    {
        if (action_cards_disabled[player])
        {	
        	action_cards_disabled[player]--;
        	if (action_cards_disabled[player]<0) action_cards_disabled[player] = 0;
        }
    }
}

void clear_attack_status(int player)
{
    for (int x = 0;x<3;x++)
    {
        if (attack_status[player][x]!=255) attack_status[player][x] = 0;
    }
}

void make_combat_ready(int player)
{
    for (int x = 0;x<3;x++)
    {
    	if (combat_ready[player][x]==254) combat_ready[player][x] = 1; 
    	if (combat_ready[player][x]==255) combat_ready[player][x] = 254; 
    	if (!combat_ready[player][x]) combat_ready[player][x] = 1;
    	if (triangle_abilities_disabled[player]!=255)
    	{
    		if (triangle_abilities_disabled[player]) triangle_abilities_disabled[player]--;
    	}
   }
}

void init_game(int hp1, int sp1, int hp2, int sp2)
{
	clear_combat_area();
	
	for (int i=0;i <2; i++)
	{
		init_hand(i);
    		init_discard_pile(i);
		draw_disabled[i] = 0;
		attack_disabled[i] = 0;
		triangle_abilities_disabled[i] = 0;
		square_abilities_disabled[i] = 0;
		circle_abilities_disabled[i] = 0;
		ac_cost_modifier[i] = 0;
		ac_cost_discounts[i] = 0;
		action_cards_disabled[i] = 0;
	}
	
	player_hp[HUMAN_PLAYER] = hp1;
	player_hp[COMPUTER_PLAYER] = hp2;
    	player_sp[HUMAN_PLAYER] = sp1;
    	player_sp[COMPUTER_PLAYER] = sp2;
}

int place_card(int pos, int player, int card_num)
{
    if (combat_area[player][pos]!=255) return 0;
    if (hand_card_type[player][card_num] == ACTION_CARD) return 0;
    
    combat_area[player][pos] = hand_cards[player][card_num]; 
    combat_area_card_type[player][pos] = hand_card_type[player][card_num];
    card_id[player][pos] = hand_cards[player][card_num]; 
    card_id_type[player][pos] = hand_card_type[player][card_num];

    if (hand_card_type[player][card_num] == SNK_CARD)
    {
        combat_area_card_hp[player][pos] = SNK_chars_hp[hand_cards[player][card_num]];
        player_sp[player] += SNK_chars_sp[hand_cards[player][card_num]];
        
        // copy backup info
        for (int i=0; i<3; i++) combat_area_backups[player][pos][i] = SNK_backups[hand_cards[player][card_num]*3+i];
    }
    
    if (hand_card_type[player][card_num] == CAPCOM_CARD)
    {
        combat_area_card_hp[player][pos] = Capcom_chars_hp[hand_cards[player][card_num]];
        player_sp[player] += Capcom_chars_sp[hand_cards[player][card_num]];  

        // copy backup info
        for (int i=0; i<3; i++) combat_area_backups[player][pos][i] = Capcom_backups[hand_cards[player][card_num]*3+i];
    }

    
    hand_cards[player][card_num] = 255;
    hand_card_type[player][card_num] = 255;

    backup_status[player][pos] = 1;
    freeze_status[player][pos] = 0;
    attack_status[player][pos] = 0;
    combat_ready[player][pos] = 0;
    num_backups[player][pos] = 0;
    counter_status[player][pos] = 0;
    card_effects[player][pos] = 0;

    set_deploy_status(player);
    
    return 1;
}

int check_for_backups(int player, int card_num)
{
	int card_type = hand_card_type[player][card_num];
	int char_num = hand_cards[player][card_num];
	int backup = 0;

	if (card_type == ACTION_CARD) return 0;

    	for (int i=0;i<3; i++)
	{
        	for (int j = 0; j<3; j++)
        	{
                	if (card_type == CAPCOM_CARD)
                	{
                		if (!backup_status[player][i]&&combat_area[player][i]!=255&&(combat_area_backups[player][i][j]  == 120 + char_num)) backup = i+1;
			}
                	else
                	{
				if (!backup_status[player][i]&&combat_area[player][i]!=255&&(combat_area_backups[player][i][j] == char_num)) backup = i+1;
                	}
		}
	}
    
	return backup;
}

int check_backup(int player, int card_num, int backup_pos)
{
    if (backup_status[player][backup_pos]) return 0;
    if (combat_area[player][backup_pos] == 255) return 0;
    
    int card_type = hand_card_type[player][card_num];
    int char_num = hand_cards[player][card_num];
    
    if (card_type == ACTION_CARD) return 0;
    
    int backup = 0;
    
    for (int i=0; i<3; i++)
    {
        if (card_type == CAPCOM_CARD)
        {
             if (combat_area_backups[player][backup_pos][i] == 120 + char_num) backup = backup_pos + 1;
        }
        else
        {
             if (combat_area_backups[player][backup_pos][i] == char_num) backup = backup_pos + 1;
        }
    }
    
    return backup;
}

void discard_card(int player, int card)
{
	int card_num = return_card(player,card);
	
	// card enters discard pile
    	discard_pile_cards[player][discard_pile_ptr[player]] = hand_cards[player][card_num];
    	discard_pile_card_type[player][discard_pile_ptr[player]] = hand_card_type[player][card_num];
	discard_pile_ptr[player]++;  

	// discard card in hand
	hand_cards[player][card_num] = 255;
	hand_card_type[player][card_num] = 255;
}

void discard_cards(int player, int card_type)
{
	while (count_cards_in_hand(player,card_type))
	{
		int card_num = 0;

		for (int i=0; i<MAX_HAND_SIZE; i++)
		{
        		if (hand_cards[player][i]!=255)
        		{
                		card_num++;
                		
                		if (hand_card_type[player][return_card(player,card_num)]==card_type)
                		{
					discard_card(player,card_num);
					count_hand(player);
                		}
        		}
		}
	}
}

void discard_card_from_pile(int player, int num_cards)
{
	for (int i=0; i<num_cards; i++)
	{
		discard_pile_cards[player][discard_pile_ptr[player]] = combat_decks[player][deck_ptr[player]];
		discard_pile_card_type[player][discard_pile_ptr[player]] = card_types[player][deck_ptr[player]];
		
		deck_ptr[player]++;
		discard_pile_ptr[player]++;
		
		if (deck_ptr[player] == NUM_CARDS) break;	
	}
}

void do_backup(int player, int card_num, int backup_pos)
{
    if (backup_status[player][backup_pos]) return;
    if (combat_area[player][backup_pos] == 255) return;

    int card_type = hand_card_type[player][card_num];
    int char_num = hand_cards[player][card_num];

    if (card_type == ACTION_CARD) return;

    for (int i=0; i<3; i++)
    {
        if (card_type == CAPCOM_CARD)
        {
             if (combat_area_backups[player][backup_pos][i] == 120 + char_num)
             {
                  backup_status[player][backup_pos] = 1;
                  combat_area_card_hp[player][backup_pos] += 300;
                  combat_area_backups[player][backup_pos][i] = 255;
		  discard_pile_cards[player][discard_pile_ptr[player]] = hand_cards[player][card_num];
    		  discard_pile_card_type[player][discard_pile_ptr[player]] = hand_card_type[player][card_num];
	          discard_pile_ptr[player]++;  
                  hand_cards[player][card_num] = 255;
                  hand_card_type[player][card_num] = 255;
                  num_backups[player][backup_pos]++;
                  break;
             }
        }
        else
        {
             if (combat_area_backups[player][backup_pos][i] == char_num)
             {
                  backup_status[player][backup_pos] = 1;
                  combat_area_card_hp[player][backup_pos] += 300;
                  combat_area_backups[player][backup_pos][i] = 255;
		  discard_pile_cards[player][discard_pile_ptr[player]] = hand_cards[player][card_num];
    		  discard_pile_card_type[player][discard_pile_ptr[player]] = hand_card_type[player][card_num];
	          discard_pile_ptr[player]++;  
                  hand_cards[player][card_num] = 255;
                  hand_card_type[player][card_num] = 255;
                  num_backups[player][backup_pos]++;
                  break;
             }
        }
    }
}

int direct_card_damage(int player, int card_pos, int damage)
{
	int opponent = 1 - player;
    	card_1_type = combat_area_card_type[player][card_pos-1];
    	card_1_num = combat_area[player][card_pos-1];
    	card_1hp = damage;
        card_b_num = 255;
        card_b_type = 255;
        card_bhp = player_hp[opponent];
    
    	animated_card_battle(player,START_ATTACK,card_pos,0,255);

        player_hp[opponent] -= damage;
        
        if (player_hp[opponent]<0) player_hp[opponent] = 0;
	
	return player;
}

void enemy_damaged_Capcom(int player, int attack_pos)
{
	if (combat_area[player][attack_pos] == 50)
	{
		// Morrigan: Life Sucker
		// Regenerate HP
		player_hp[player]+=combat_area_card_hp[player][attack_pos];
	}

	if (combat_area[player][attack_pos] == 64)
	{
		// Jedah: P D C
		// Draw Card
		draw_card(player);
		if (player == HUMAN_PLAYER) draw_card_animated(player);
	}
								
}

void enemy_damaged_SNK(int player, int attack_pos)
{
	int opponent = 1 - player;
	
	if (combat_area[player][attack_pos] == 63)
	{
		//Heidern: S-Bringer
		//Steal Up to 3 SP from enemy when Heidern
		//Damages Him
		if (player_sp[opponent]<3)
		{
			player_sp[player] += player_sp[opponent];
			player_sp[opponent] = 0;
		}
		else
		{
			player_sp[player] += 3;
			player_sp[opponent] -= 3;
		}
	}
}

void enemy_damaged(int player, int attack_pos)
{
	if (circle_abilities_disabled[player]) return;
	
	if (combat_area_card_type[player][attack_pos]==SNK_CARD) enemy_damaged_SNK(player,attack_pos);
	if (combat_area_card_type[player][attack_pos]==CAPCOM_CARD) enemy_damaged_Capcom(player,attack_pos);
}


void post_combat_effects_Capcom(int player, int attack_pos)
{
	if (combat_area[player][attack_pos] == 30)
	{
		// Karin: Hon Shou
		// Deals 200 pts damage to Enemy when this character is blocked
		direct_card_damage(player,attack_pos+1,200);
	}
}

void post_combat_effects_SNK(int player, int attack_pos)
{
	if (combat_area[player][attack_pos] == 30)
	{
		// Haohmaru: Iron Slice
		// Give Enemy overflow damage when this character is blocked
		direct_card_damage(player,attack_pos+1,combat_area_card_hp[player][attack_pos]);
	}
}

void post_combat_effects(int player, int attack_pos)
{
	if (circle_abilities_disabled[player]) return;
	
	if (!combat_area_card_hp[player][attack_pos]||combat_area[player][attack_pos]==255) return;
	if (combat_area_card_type[player][attack_pos]==SNK_CARD) post_combat_effects_SNK(player,attack_pos);
	if (combat_area_card_type[player][attack_pos]==CAPCOM_CARD) post_combat_effects_Capcom(player,attack_pos);
}

int card_battle(int player, int attack_pos, int block_pos)
{
    int opponent = 1 - player;

    card_1_type = combat_area_card_type[player][attack_pos-1];
    card_1_num = combat_area[player][attack_pos-1];
    card_1hp = get_hp(player,attack_pos-1);

    if (block_pos!=255)
    {
        card_b_num = combat_area[opponent][block_pos-1];
        card_b_type = combat_area_card_type[opponent][block_pos-1];
        card_bhp = get_hp(opponent,block_pos-1);
        if (check_xcopy(opponent,block_pos-1)) card_bhp = card_1hp;
    }
    else
    {
        card_b_num = 255;
        card_b_type = 255;
        card_bhp = player_hp[opponent];
    } 
    
    animated_card_battle(player,START_ATTACK,attack_pos,0,block_pos);

    attack_pos --;   // convert 1-3 to 0-2

    int player_card_hp = combat_area_card_hp[player][attack_pos];

    freeze_status[player][attack_pos] = 1;
    attack_status[player][attack_pos] = 1; 

    if (block_pos == 255)
    {
        player_hp[opponent] -= player_card_hp;
	attack_status[player][attack_pos] = 1; 
        if (player_hp[opponent]<0) player_hp[opponent] = 0;
        enemy_damaged(player,attack_pos);
        return player;
    }

    block_pos --;    // convert 1-3 to 0-2

    freeze_status[opponent][block_pos] = 1;
    counter_status[opponent][block_pos] = 1;
    
    int opponent_card_hp = combat_area_card_hp[opponent][block_pos];

    if (check_xcopy(opponent,block_pos)) opponent_card_hp = player_card_hp;
    
    // perform actual card combat
    combat_area_card_hp[player][attack_pos] -= opponent_card_hp;
    combat_area_card_hp[opponent][block_pos] -= player_card_hp;
    
    if (combat_area_card_hp[player][attack_pos]<=0)
    {
        put_to_discard_pile(player,attack_pos);
        kill_character(player,attack_pos);
    }
    else
    {
    	post_combat_effects(player,attack_pos);
    }
    
    if (combat_area_card_hp[opponent][block_pos]<=0)
    {
        put_to_discard_pile(opponent,block_pos);
        kill_character(opponent,block_pos);
    }

    if (!combat_area_card_hp[player][attack_pos]&&!combat_area_card_hp[opponent][block_pos]) return BOTH_PLAYERS; 
    
    if (!combat_area_card_hp[player][attack_pos]) return opponent;
    
    return player;
}

int united_attack(int player, int attack_pos1, int attack_pos2, int block_pos)
{
    int opponent = 1 - player;
    player_sp[player] -= 5;

    card_1_type = combat_area_card_type[player][attack_pos1-1];
    card_1hp = get_hp(player,attack_pos1-1);
    card_1_num = combat_area[player][attack_pos1-1];

    card_2_type = combat_area_card_type[player][attack_pos2-1];
    card_2hp = get_hp(player,attack_pos2-1);
    card_2_num = combat_area[player][attack_pos2-1];
    
    if (block_pos!=255)
    {	
    	card_b_type = combat_area_card_type[opponent][block_pos-1];
    	card_b_num = combat_area[opponent][block_pos-1];
        card_bhp = get_hp(opponent,block_pos-1);
        if (check_xcopy(opponent,block_pos-1)) card_bhp = card_1hp + card_2hp;
        opp_hp = player_hp[opponent];
    }
    else
    {
    	card_b_type = 255;
    	card_b_num = 255;
        card_bhp = player_hp[opponent];
    }
    
    animated_card_battle(player,START_ATTACK,attack_pos1,attack_pos2,block_pos);

    attack_pos1 --;
    attack_pos2 --;
    
    int player_card_hp1 = get_hp(player,attack_pos1);
    int player_card_hp2 = get_hp(player,attack_pos2);   
    int total_player_card_hp = player_card_hp1+player_card_hp2;

    attack_status[player][attack_pos1] = 1;
    attack_status[player][attack_pos2] = 1;
    freeze_status[player][attack_pos1] = 1;
    freeze_status[player][attack_pos2] = 1;
    
    if (block_pos == 255)
    {
        player_hp[opponent] -= total_player_card_hp;
        if (player_hp[opponent]<0) player_hp[opponent] = 0;
        enemy_damaged(player,attack_pos1);
        
        return player;   
    }
    
    block_pos --;
    
    int invincible_to_united_attack = united_invincible(opponent,block_pos);
    
    counter_status[opponent][block_pos] = 1;
    freeze_status[opponent][block_pos] = 1;
    
    int opponent_card_hp = combat_area_card_hp[opponent][block_pos];             
    if (check_xcopy(opponent,block_pos)) opponent_card_hp = total_player_card_hp;
    
    // perform actual card combat
    int united_attack_hp = total_player_card_hp - opponent_card_hp;
    combat_area_card_hp[opponent][block_pos] -= total_player_card_hp;
    
    if (combat_area_card_hp[opponent][block_pos]<=0)
    {
        put_to_discard_pile(opponent,block_pos);
        kill_character(opponent,block_pos);
    }
    
    if (united_attack_hp<=0)
    {
        put_to_discard_pile(player,attack_pos1);
        put_to_discard_pile(player,attack_pos2);
        kill_character(player,attack_pos1);
        kill_character(player,attack_pos2);
    }
    else    
    {
	if (!invincible_to_united_attack)
        {
        	player_hp[opponent] -= united_attack_hp;
        	if (player_hp[opponent]<0) player_hp[opponent] = 0;
        }

        if (opponent_card_hp>=player_card_hp2)
        {
        	put_to_discard_pile(player,attack_pos2);
                kill_character(player,attack_pos2);
                combat_area_card_hp[player][attack_pos1] = united_attack_hp;
        }
        else
        {
        	combat_area_card_hp[player][attack_pos2] = player_card_hp2 - opponent_card_hp;
        }
    }
    
    if (!combat_area_card_hp[player][attack_pos1]&&!combat_area_card_hp[opponent][block_pos])
    {
        return BOTH_PLAYERS;
    }
    
    if (!combat_area_card_hp[player][attack_pos1]) return opponent;

    return player;
}

void do_computer_use_special_card()
{
	if (action_cards_disabled[COMPUTER_PLAYER]) return;
}

void do_special_effects_SNK(int player, int card_pos)
{
	if (card_effects[player][card_pos]==2) return;
	card_effects[player][card_pos]++;
	
	int card_num = combat_area[player][card_pos];
	int opponent = 1 - player;

	if (card_num == 0)
	{
		// Terry: Terry Rush
		// Keeps Character Out Of Freeze Phase
		if (attack_status[player][card_pos]||counter_status[player][card_pos])
		{
			freeze_status[player][card_pos] = 0;
			attack_status[player][card_pos] = 0;
			counter_status[player][card_pos] = 0;
			attack_status[player][card_pos] = 0;
			combat_ready[player][card_pos] = 0;
			defrost(player,card_pos);
		}
		card_effects[player][card_pos] = 0;
	}

	if (card_num == 1)
	{
		// Andy: Shadow Slicer
		// Enemy Randomly Discards a Card
		if (combat_ready[player][card_pos] == 0)
		{
			// Andy just entered play ...
			// so enemy randomly discards a card
			count_hand(opponent);
			int random_card = rand()%hand_size[opponent]+1;
			discard_card(opponent,random_card);
			discard_card_animated(opponent);
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 4)
	{
		// Billy: Shrike Drop
		// Select 1 action card from enemy's hand. Opponent
		// Discards that card
		if (combat_ready[player][card_pos] == 0)
		{
			count_hand(opponent);
			
			if (hand_size[opponent])
			{
				if (player == HUMAN_PLAYER)
				{
					if (count_cards_in_hand(opponent,ACTION_CARD))
					{
						int selected_card = 0;
						
						while (!selected_card)
						{
							selected_card = select_card_from_hand(opponent,HUMAN_MODE);
							int card_data = return_card(opponent,selected_card);
							if (hand_card_type[opponent][card_data]!=ACTION_CARD) selected_card = 0;
						}
						
						discard_card(opponent,selected_card);
						discard_card_animated(opponent);
					}
					else
					{
						select_card_from_hand(opponent,HUMAN_BROWSE_MODE);
					}
				}
				else
				{
					if (count_cards_in_hand(opponent,ACTION_CARD))
					{
						int selected_card = select_card_from_hand(opponent,AI_SELECT_STRONG_ACTION_MODE);
						discard_card(opponent,selected_card);
						discard_card_animated(opponent);
					}
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 6)
	{
		// Duck King: Duck Dance
		// Raise a characters HP by 300 pts
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_other_chars(player,card_pos);
			if (char_count == 0)
			{
				lightningv(player,card_pos);
				combat_area_card_hp[player][card_pos] += 300;
			}
			else
			{
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO POWER UP$"};
					int selected_char = select_char(player,select_character,SELECT_ANY);
					if (selected_char>10)
					{
						lightningv(opponent,selected_char-11);
						combat_area_card_hp[opponent][selected_char-11] += 300; 
					}
					else
					{ 
						lightningv(player,selected_char-1);
						combat_area_card_hp[player][selected_char-1]+=300;
					}
				}
				else
				{
					char select_character [] = {"CHARACTER/POWERED$"};
					int selected_char = select_char(player,select_character,AI_SELECT_OWN);
					lightningv(player,selected_char-1);
					combat_area_card_hp[player][selected_char-1]+=300;
				}
			}
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 10)
	{
		// Yamazaki: Face Off
		// Freezes another character
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_other_chars(player,card_pos);		
			if (char_count)
			{
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO FREEZE$"};
					int selected_char = 0;
					
					while (!selected_char||selected_char==card_pos+1)
					{
						selected_char = select_char(player,select_character,SELECT_ANY);
					}
					
					if (selected_char>10) freeze_status[opponent][selected_char-11]=1; else freeze_status[player][selected_char-1]=1;
				}
				else
				{
					char select_character [] = {"CARD FROZEN$"};
					int selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
					freeze_status[opponent][selected_char-1]=1; 
				}
			}
			
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 15)
	{
		// Alfred: W Rider
		// Lets this character attack this turn
		if (combat_ready[player][card_pos] == 0)
		{
			// Activate Alfred so he can attack this turn
			combat_ready[player][card_pos] = 1;
			twinkle(player,card_pos);
		}
		card_effects[player][card_pos] = 0;
	}
	
	if (card_num == 24)
	{
		// King: Dealer
		// Discard entire hand
		// Select character card from deck and add it to your
		// hand. Shuffle Deck Afterwards
		
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = 0;
			char_count = count_cards_in_deck(player,SNK_CARD);
			char_count += count_cards_in_deck(player,CAPCOM_CARD);
		
			if (char_count)
			{
				while (hand_size[player])
        			{
        				discard_card(player,hand_size[player]);
        				count_hand(player);
				}

				discard_card_animated(player);

				int selected_card = 0;
				
				if (player == HUMAN_PLAYER)
				{
					selected_card = select_card_from_deck(player,ANY_CHAR_CARD,HUMAN_MODE);
				}
				else
				{
					selected_card = select_card_from_deck(player,ANY_CHAR_CARD,AI_SELECT_STRONG_MODE);
				}
				
				put_on_top(player,selected_card-1);
				draw_card(player);
				draw_card_animated(player);
				shuffle_deck(player);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 28)
	{
		// Eiji : Prophecy
		// Rearraange UP the top 3 cards of your
		// deck in any order
		if (combat_ready[player][card_pos] == 0)
		{
			if (player == HUMAN_PLAYER)
			{
				int cards = NUM_CARDS - deck_ptr[opponent];
				if (cards>3) cards = 3;
				if (cards<1) cards = 0;
				
				int arrange_cards = 0;
				int choice = 0;

				char msg [] = {"CHOOSE ACTION$"};
				char target_001 [] = {"ARRANGE DECK$"};
				char target_002 [] = {"NO ACTION$"};
				char *target_menu [] = {target_001,target_002};
				
				if (cards>1)
				{
					choice = two_choice_menu(player,card_pos,msg,target_menu);
					if (choice != 2) arrange_cards = 1; else arrange_cards = 0;
				}
				
				while (arrange_cards&&(cards>1))
				{
					char target2_001 [] = {"PUT ON TOP$"};
					char target2_002 [] = {"NO ACTION$"};
					char *target_menu2 [] = {target2_001,target2_002};
					int selected_card = view_top_cards_from_deck(player,cards,HUMAN_MODE);
					
					choice = two_choice_menu2(player,msg,target_menu2,selected_card-1,DECK_PILE_TYPE);
				
					if (choice == 1)
					{
						put_on_top(player,selected_card-1);
					}
					
					choice = two_choice_menu(player,card_pos,msg,target_menu);
					if (choice != 2) arrange_cards = 1; else arrange_cards = 0;
				}
			}
			else
			{
				int ai_value[3];
				
				for (int i=0;i<3;i++) ai_value[i] = 0;
				
				int cards = NUM_CARDS - deck_ptr[opponent];
				if (cards>3) cards = 3;
				if (cards<1) cards = 0;
				
				if (cards>1)
				{
					for (int i=0; i<cards; i++)
					{
						int adj = 0;
					
						if (card_types[player][deck_ptr[player]+i]!=ACTION_CARD)
						{
							for (int j=0; j<3; j++)
							{
								int value = check_card_in_deck_ai_value(player,deck_ptr[player]+i,j);
								if (value) adj ++;
								ai_value[i] += value;
							}
						}
						else
						{
							adj = 1;
							ai_value[i] = check_card_in_deck_ai_value(player,deck_ptr[player]+i,AI_ABILITY);
						}
					
						ai_value[i] = ai_value[i]/adj;
					}
				}
				
				if (cards>1)
				{
					int changes=1;
				
					while(changes)
					{
						changes = 0;
						
						for (int i=0; i<(cards-1); i++)
						{
							if (ai_value[i]<ai_value[i+1])
							{
								changes = 1;
								int temp = ai_value[i];
								ai_value[i] = ai_value[i+1];
								ai_value[i+1] = temp;
								
								temp = combat_decks[player][deck_ptr[player]+i];
								combat_decks[player][deck_ptr[player]+i] = combat_decks[player][deck_ptr[player]+i+1];
								combat_decks[player][deck_ptr[player]+i+1] = temp;
								
								temp = card_types[player][deck_ptr[player]+i];
								card_types[player][deck_ptr[player]+i] = card_types[player][deck_ptr[player]+i+1];
								card_types[player][deck_ptr[player]+i+1] = temp;
							}
						}
					}
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 31)
	{
		// Ukyo: Dwindling Life
		// KOs this character at the end of the turn
		// if he attacks or counter attacks
		if (attack_status[player][card_pos]||counter_status[player][card_pos])
		{
			// If Ukyo Attacks/Blocks KO him
			bloodsplat(player,card_pos);
			put_to_discard_pile(player,card_pos);
			kill_character(player,card_pos);
		}
		card_effects[player][card_pos] = 0;
	}

	if (card_num == 33)
	{
		// Galford: Hey Puppy
		// Draws 1 Card
		// Galford Just Entered Play so draw 1 card
		if (combat_ready[player][card_pos]==0)
		{
			draw_card(player);
			if (player == HUMAN_PLAYER) draw_card_animated(player);
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 34)
	{
		// Nakoruru C: Balm of Nature
		// Get 500 HP
		if (combat_ready[player][card_pos]==0)
		{
			player_hp[player]+=500;
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 35)
	{
		// Nakoruru T: Shikuroo's Fang
		// After your turn ends, gives all  "frozen" characters in enemy's ring 200 pts damage.
		if (card_effects[player][card_pos]==2)
		{
			// End Of Turn
			for (int i=0;i<3; i++)
			{
				if (combat_area[opponent][i]!=255&&freeze_status[opponent][i])
				{
					explode_card(opponent,i);
					combat_area_card_hp[opponent][i] -= 200;
					if (combat_area_card_hp[opponent][i]<=0)
					{
						bloodsplat(opponent,i);
						put_to_discard_pile(opponent,i);
					        kill_character(opponent,i);
					}
				}
			}
			
			card_effects[player][card_pos] = 1;
		}
	}
	
	if (card_num == 41)
	{
		// Kyoshiro: Warror Dance
		// raises the BP of all characters in 
		// your ring by 100 pts
		if (combat_ready[player][card_pos]==0)
		{
			for (int i=0; i<3; i++)
			{
				if (combat_area[player][i]!=255)
				{
					lightningv(player,i);
					combat_area_card_hp[player][i]+=100;
				}			
			}
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 45 || card_num == 75)
	{
		// Genjuro: I know it!
		// Iori: Blood Contract
		// Disable this character's counter attack
		counter_status[player][card_pos] = 255;
		card_effects[player][card_pos] = 0;
	}

	if (card_num == 46)
	{
		// Nicotine: Exorcism Card
		// Neutralizes all square abilities
		card_effects[player][card_pos] = 1;
		square_abilities_disabled[player] = 255;
		square_abilities_disabled[opponent] = 255;
	}

	if (card_num == 51)
	{
		// Sougetsu: Frigid Smirk
		// Enemy skips draw phase until his next turn
		if (combat_ready[player][card_pos] == 0)
		{
			draw_disabled[opponent] = 1;
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 53)
	{
		// Shiki (T): Yuga's spell
		// Raises action card costs by 3 SP
		
		if (combat_ready[player][card_pos]==0)
		{
			ac_cost_modifier[player] += 3;
			ac_cost_modifier[opponent] += 3;
		}
		else
		{
			if (!ac_cost_modifier[player]) ac_cost_modifier[player] += 3;
			if (!ac_cost_modifier[opponent]) ac_cost_modifier[opponent] += 3;
		}
		
		card_effects[player][card_pos] = 1;
	}

	if (card_num == 54)
	{
		// Asra: Rising Evil
		// Return 1 discarded card to deck
		// Shuffle deck afterwards
		
		if (combat_ready[player][card_pos]==0)
		{
			if (discard_pile_ptr[player])
			{
				int selected_card = 0;
				
				if (player == HUMAN_PLAYER)
				{
					selected_card = select_card_from_discard_pile(player,ANY_CARD,HUMAN_MODE);
				}
				else
				{
					selected_card = selected_card = select_card_from_discard_pile(player,ANY_CHAR_CARD,AI_SELECT_STRONG_MODE);
				}

				put_to_bottom_of_discard(player,selected_card-1);
				return_to_top_of_deck_from_bottom_of_discard(player);
				shuffle_deck(player);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 48)
	{
		// Gaira: Shout!
		// Ends all character's freeze phase
		if (combat_ready[player][card_pos] == 0)
		{
			for (int j=0;j<2;j++)
			{
				for (int i=0;i<3;i++)
				{
					if (combat_area[j][i]!=255)
					{
						if (freeze_status[j][i]||attack_status[j][i]||(counter_status[j][i]&&counter_status[j][i]!=255))
						{
							defrost(j,i);
							combat_ready[j][i] = 0;
						}
					}
				}	
			}
			
			clear_attack_status(HUMAN_PLAYER);
			clear_attack_status(COMPUTER_PLAYER);
			clear_freeze_status(HUMAN_PLAYER);
			clear_freeze_status(COMPUTER_PLAYER);
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 56)
	{
		// Amakusa: Give Yourself!
		// KO's all other characters in your ring
		// Raises SP by 3 for each KOed character
		if (combat_ready[player][card_pos] == 0)
		{
			for (int i=0; i<3; i++)
			{
				if (i!=card_pos)
				{
					if (combat_area[player][i]!=255)
					{
						bloodsplat(player,i);
						put_to_discard_pile(player,i);
						kill_character(player,i);
						player_sp[player]+=3;
					}
				}
			}
			
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 58)
	{
		// The Ump: Mystic
		// Disable this character's attacks, counter attack
		counter_status[player][card_pos] = 255;
		attack_status[player][card_pos] = 255;
		card_effects[player][card_pos] = 0;
	}

	if (card_num == 59)
	{
		// Kyo: Orochi Wave
		// Decreases Enemy Life by 500 pts
		if (combat_ready[player][card_pos]==0) direct_card_damage(player,card_pos+1,500);
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 60)
	{
		// Kyo (P): 182 Ways
		// Decreases Enemy Character or Player HP by 300
		card_effects[player][card_pos] = 2;
		if (combat_ready[player][card_pos]==0)
		{
			int char_count = count_chars(opponent);
			if (char_count)
			{
				if (player == HUMAN_PLAYER)
				{
					char msg [] = {"SELECT TARGET$"};
					char target_001 [] = {"PLAYER$"};
					char target_002 [] = {"CHARACTER$"};
					char *target_menu [] = {target_001,target_002};

					int choice = two_choice_menu(player,card_pos,msg,target_menu);
					
					if (choice == 1)
					{
						direct_card_damage(player,card_pos+1,300);
					}
					else
					{
						char select_character [] = {"SELECT/CHARACTER$"};
						int selected_char = select_char(player,select_character,SELECT_ENEMY);
						
						explode_card(opponent,selected_char-1);
						combat_area_card_hp[opponent][selected_char-1]-=300;
						if (combat_area_card_hp[opponent][selected_char-1]<=0)
						{
							bloodsplat(opponent,selected_char-1);
							put_to_discard_pile(opponent,selected_char-1);
							kill_character(opponent,selected_char-1);
						}
					}
				}
				else
				{
					if (count_chars(COMPUTER_PLAYER)>count_chars(HUMAN_PLAYER))
					{
						direct_card_damage(player,card_pos+1,300);
					}
					else
					{
						char select_character [] = {"CHARACTER/DAMAGED$"};
						int selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
						
						explode_card(opponent,selected_char-1);
						combat_area_card_hp[opponent][selected_char-1]-=300;
						if (combat_area_card_hp[opponent][selected_char-1]<=0)
						{
							bloodsplat(opponent,selected_char-1);
							put_to_discard_pile(opponent,selected_char-1);
							kill_character(opponent,selected_char-1);
						}
					}
				}
			}
			else
			{
				direct_card_damage(player,card_pos+1,300);
			}
		}
	}

	if (card_num == 64)
	{
		// Leona: XCALIBUR
		// Gives 200 pts damage to one other character
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_chars(player);
			int enemy_count = count_chars(opponent);
			
			int selected_char = 0;

			if (player == HUMAN_PLAYER)
			{
				if (char_count>1||enemy_count)
				{
					char select_character [] = {"SELECT/CHARACTER$"};
					while (!selected_char||selected_char == card_pos+1)
					{
						selected_char = select_char(player,select_character,SELECT_ANY);
					}
					
					if (selected_char>10)
					{
						explode_card(opponent,selected_char-11);
						combat_area_card_hp[opponent][selected_char-11]-=200;
						if (combat_area_card_hp[opponent][selected_char-11]<=0)
						{
							bloodsplat(opponent,selected_char-11);
							put_to_discard_pile(opponent,selected_char-11);
					        	kill_character(opponent,selected_char-11);
						}
					}
					else 
					{
						explode_card(player,selected_char-1);
						combat_area_card_hp[player][selected_char-1]-=200;
						if (combat_area_card_hp[player][selected_char-1]<=0)
						{
							bloodsplat(player,selected_char-1);
							put_to_discard_pile(player,selected_char-1);
					        	kill_character(player,selected_char-1);
						}
					}
				}
			}
			else
			{
				if (char_count>1||enemy_count)
				{
					char select_character [] = {"CHARACTER/DAMAGED$"};
					if (enemy_count)
					{
						selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
						explode_card(opponent,selected_char-1);
						combat_area_card_hp[opponent][selected_char-1]-=200;
						if (combat_area_card_hp[opponent][selected_char-1]<=0)
						{
							bloodsplat(opponent,selected_char-1);
							put_to_discard_pile(opponent,selected_char-1);
							kill_character(opponent,selected_char-1);
						}
					}
					else
					{
						selected_char = select_char(player,select_character,AI_SELECT_WEAK);
						explode_card(player,selected_char-1);
						combat_area_card_hp[player][selected_char-1]-=200;
						if (combat_area_card_hp[player][selected_char-1]<=0)
						{
							bloodsplat(player,selected_char-1);
							put_to_discard_pile(player,selected_char-1);
							kill_character(player,selected_char-1);
						}
					}
				}
			}
		}
			
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 68)
	{
		// Kensu: Meat Muffin
		// Get 200 HPs
		if (combat_ready[player][card_pos]==0) player_hp[player]+=200;
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 71)
	{
		// Choi: Enabler
		// Shuffle your deck or your enemy'
		if (combat_ready[player][card_pos]==0)
		{
			if (player == HUMAN_PLAYER)
			{
				char msg [] = {"SELECT DECK$"};
				char target_001 [] = {"YOURs$"};
				char target_002 [] = {"OPPONENTs$"};
				char *target_menu [] = {target_001,target_002};

				int choice = two_choice_menu(player,card_pos,msg,target_menu);
					
				if (choice == 1)
				{
					shuffle_deck(player);
				}
				else
				{
					shuffle_deck(opponent);
				}
			}
			else
			{
				count_hand(COMPUTER_PLAYER);
				
				int avg_ai = 0;
				
				for (int i=0; i<hand_size[COMPUTER_PLAYER]; i++)
				{
					int card = return_card(COMPUTER_PLAYER,i+1);
					if (hand_card_type[COMPUTER_PLAYER][card]!=ACTION_CARD)
					{
						int combat_ai = check_card_ai_value(COMPUTER_PLAYER,i+1,AI_COMBAT);
						int deploy_ai = check_card_ai_value(COMPUTER_PLAYER,i+1,AI_DEPLOY);
						if (combat_ai>deploy_ai) avg_ai += combat_ai; else avg_ai += deploy_ai;
					}
					else
					{
						avg_ai += check_card_ai_value(COMPUTER_PLAYER,i+1,AI_ABILITY);
					}
				}
				
				if (avg_ai<(hand_size[COMPUTER_PLAYER]*5)) shuffle_deck(player); else shuffle_deck(opponent);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 72)
	{
		// Saishu: Exorcism
		// Decreases Enemy Life by 200 pts
		if (combat_ready[player][card_pos]==0)
		{
			direct_card_damage(player,card_pos+1,200);
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 76)
	{
		// Yashiro: Counterblow
		// Returns 1 other character to his hand
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_chars(opponent);
			if (char_count)
			{
				int selected_char = 0;
			
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO EXPEL$"};
					selected_char = select_char(player,select_character,SELECT_ENEMY);
				}
				else
				{
					char select_character [] = {"CHARACTER/EXPELLED$"};
					selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
				}
			
				return_to_hand(opponent,selected_char-1);
			}
		}
		
		card_effects[player][card_pos] = 2;	
	}

	if (card_num == 77)
	{
		// Shermie: Be Gentle to me
		// Disable 1 enemy character's attack until enemy's next turn
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_chars(opponent);
			if (char_count)
			{
				int selected_char = 0;
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT/CHARACTER$"};
					selected_char = select_char(player,select_character,SELECT_ENEMY);
				}
				else
				{
					char select_character [] = {"ATTACK/DISABLED$"};
					selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
				}

				combat_ready[opponent][selected_char-1] = 255;
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 80)
	{
		// Orochi: Sanity
		// Reduces each player's SP to 0
		if (combat_ready[player][card_pos]==0)
		{
			player_sp[opponent] =0;
			player_sp[player] =0;
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 83)
	{
		// Wild Iori: Kurf
		// KOs one other character whose BP is 800 or more
		// or this character if none is available
		card_effects[player][card_pos] = 2;

		if (combat_ready[player][card_pos] == 0)
		{
			int found_char = 0;
			
			for (int i=0; i<3; i++)
			{
				if (!found_char)
				{
					if (combat_area[opponent][i]!=255&&get_hp(opponent,i)>=800) found_char = i+1;
				}
			}
			
			if (found_char)
			{
				bloodsplat(opponent,found_char-1);
			        put_to_discard_pile(opponent,found_char-1);
        			kill_character(opponent,found_char-1);
			}
			else
			{
				for (int i=0; i<3; i++)
				{
					if (!found_char)
					{
						if (combat_area[player][i]!=255&&i!=card_pos&&get_hp(player,i)>=800) found_char = i+1;
					}
				}
				
				if (found_char)
				{
					bloodsplat(player,found_char-1);
			        	put_to_discard_pile(player,found_char-1);
        				kill_character(player,found_char-1);
				}
				else
				{
					bloodsplat(player,card_pos);
			        	put_to_discard_pile(player,card_pos);
        				kill_character(player,card_pos);
				}
			}
		}	
	}
	
	if (card_num == 84)
	{
		// Wild Leona: Awakening
		// Deals 400 pts damage to another character
		// (or this character if another is unavailable)
		if (combat_ready[player][card_pos] == 0)
		{
			int enemy_count = count_chars(opponent);
			int allies_count = count_chars(player);
			int selected_char = 0;

			if (enemy_count==0&&allies_count==1)
			{
				explode_card(player,card_pos);
				combat_area_card_hp[player][card_pos] -= 400;
				if (combat_area_card_hp[player][card_pos] <= 0)
				{
					bloodsplat(player,card_pos);
			        	put_to_discard_pile(player,card_pos);
        				kill_character(player,card_pos);
				}
			}
			else
			{
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT/CHARACTER$"};
					while (!selected_char||selected_char==card_pos+1)
					{
						selected_char = select_char(player,select_character,SELECT_ANY);
					}
					
					if (selected_char>10)
					{
						explode_card(opponent,selected_char-11);
						combat_area_card_hp[opponent][selected_char-11] -= 400;
						if (combat_area_card_hp[opponent][selected_char-11]<=0)
						{
							bloodsplat(opponent,selected_char-11);
			        			put_to_discard_pile(opponent,selected_char-11);
        						kill_character(opponent,selected_char-11);
						}
					}
					else
					{
						explode_card(player,selected_char-1);
						combat_area_card_hp[player][selected_char-1] -= 400;
						if (combat_area_card_hp[player][selected_char-1]<=0)
						{
							bloodsplat(player,selected_char-1);
			        			put_to_discard_pile(player,selected_char-1);
        						kill_character(opponent,selected_char-1);
						}
					}
				}
				else
				{
					char select_character [] = {"CHARACTER/DAMAGED$"};
					
					if (enemy_count)
					{
						selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
						explode_card(opponent,selected_char-1);
						combat_area_card_hp[opponent][selected_char-1] -= 400;
						if (combat_area_card_hp[opponent][selected_char-1]<=0)
						{
							bloodsplat(opponent,selected_char-1);
			        			put_to_discard_pile(opponent,selected_char-1);
        						kill_character(opponent,selected_char-1);
						}
					}
					else
					{
						selected_char = select_char(player,select_character,AI_SELECT_WEAK);
						explode_card(player,selected_char-1);
						combat_area_card_hp[player][selected_char-1] -= 400;
						if (combat_area_card_hp[player][selected_char-1]<=0)
						{
							bloodsplat(player,selected_char-1);
			        			put_to_discard_pile(player,selected_char-1);
        						kill_character(player,selected_char-1);
						}
					}
				}
			}
			
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 85)
	{
		// K: Unstoppable
		// All other characters get 300 pts damage
		if (combat_ready[player][card_pos]==0)
		{
			for (int i=0;i<3; i++)
			{
				if (combat_area[opponent][i]!=255)
				{
					explode_card(opponent,i);
					combat_area_card_hp[opponent][i] -= 300;
					if (combat_area_card_hp[opponent][i]<=0)
					{
						bloodsplat(opponent,i);
						put_to_discard_pile(opponent,i);
					        kill_character(opponent,i);
					}
				}
				
				if (i!=card_pos&&combat_area[player][i]!=255)
				{
					explode_card(player,i);
					combat_area_card_hp[player][i] -= 300;
					if (combat_area_card_hp[player][i]<=0)
					{
						bloodsplat(player,i);
						put_to_discard_pile(player,i);
					        kill_character(player,i);
					}
				}
			}
			
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 88)
	{
		// Kaede: Power Match
		// Uses SP at the beginning of your turn
		if (card_effects[player][card_pos]==1&&combat_ready[player][card_pos])
		{
			player_sp[player] -= 2;
			if (player_sp[player]<=0) player_sp[player] = 0;
		}
		else
		{
			if (card_effects[player][card_pos]==2) card_effects[player][card_pos] = 0;
		}
	}
	
	if (card_num == 103)
	{
		// Wanderer: Change
		// Copies one enemy character HP when present
		if (combat_ready[player][card_pos]==0)
		{
			int char_count = count_chars(opponent);
			
			if (char_count)
			{
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"COPY/CHARACTER$"};
					int selected_char = select_char(player,select_character,SELECT_ENEMY);
					lightningv(player,card_pos);
					combat_area_card_hp[player][card_pos] = combat_area_card_hp[opponent][selected_char-1];
				}
				else
				{
					char select_character [] = {"CHARACTER/COPIED$"};
					int selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
					lightningv(player,card_pos);
					combat_area_card_hp[player][card_pos] = combat_area_card_hp[opponent][selected_char-1];
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
}

void do_special_effects_Capcom(int player, int card_pos)
{
	if (card_effects[player][card_pos]>=2) return;
	card_effects[player][card_pos]++;

	int card_num = combat_area[player][card_pos];
	int opponent = 1 - player;

	if (card_num == 1)
	{
		// Ken: Rage Wave
		// Keeps Character Out of Freeze Phase
		if (attack_status[player][card_pos]||counter_status[player][card_pos])
		{
			freeze_status[player][card_pos] = 0;
			attack_status[player][card_pos] = 0;
			counter_status[player][card_pos] = 0;
			attack_status[player][card_pos] = 0;
			combat_ready[player][card_pos] = 0;
			defrost(player,card_pos);
		}
		card_effects[player][card_pos] = 1;
	}
	
	if (card_num == 2)
	{
		// Chun Li:  S B Kick
		// Use all SP. All character HP becomes 100 except this one
		if (combat_ready[player][card_pos]==0)
		{
			player_sp[player] = 0;
			for (int i=0; i<3; i++)
			{
				if (combat_area[opponent][i]!=255&&get_hp(opponent,i)!=100)
				{
					combat_area_card_hp[opponent][i] = 100;
					firezap(opponent,i);
				}
				
				if (combat_area[player][i]!=255&&i!=card_pos&&get_hp(player,i)!=100)
				{
					combat_area_card_hp[player][i] = 100;
					firezap(player,i);
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 3)
	{
		// Guile: S. Kick
		// Neutralizes all triangle abilities
		card_effects[player][card_pos] = 0;
		triangle_abilities_disabled[player] = 255;
		triangle_abilities_disabled[opponent] = 255;
	}
	
	if (card_num == 6 || card_num == 16)
	{
		// Dhalsim: Enlightenment
		// See Deck's top card. Can be put at the bottom
		
		// Cammy (A): Spy
		// See Enemy Deck's top card. Can be put at the bottom
		
		if (combat_ready[player][card_pos] == 0)
		{
			if (player == HUMAN_PLAYER)
			{
				// Check if Cammy (A) enters the ring
				if (card_num == 16) player = opponent;
				
				char msg [] = {"PUT TO$"};
				char target_001 [] = {"BOTTOM$"};
				char target_002 [] = {"NO ACTION$"};
				char *target_menu [] = {target_001,target_002};

				int choice = two_choice_menu2(player,msg,target_menu,deck_ptr[player],DECK_PILE_TYPE);
				
				if (choice == 1)
				{
					put_to_bottom(player,deck_ptr[player]);
				}
			}
			else
			{
				int avg_ai = 0;
				int deck_owner = player;
				if (card_num == 16) deck_owner = opponent;

				int card = deck_ptr[deck_owner];
				
				if (card_types[deck_owner][card]!=ACTION_CARD)
				{
					int combat_ai = check_card_in_deck_ai_value(deck_owner,card,AI_COMBAT);
					int deploy_ai = check_card_in_deck_ai_value(deck_owner,card,AI_DEPLOY);
					if (combat_ai>deploy_ai) avg_ai += combat_ai; else avg_ai += deploy_ai;
				}
				else
				{
					avg_ai += check_card_in_deck_ai_value(deck_owner,card,AI_DEPLOY);
				}
			
				if (card_num == 16)
				{
					if (avg_ai>5) put_to_bottom(deck_owner,card);
				}
				else
				{
					if (avg_ai<5) put_to_bottom(deck_owner,card);
				}
			}
		}
		
		if (card_num == 16) player = 1 - opponent;
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 10)
	{
		// Sagat: True Power
		// Each player reveals her hand and discards all action cards
		//
		if (combat_ready[player][card_pos] == 0)
		{
			if (player == HUMAN_PLAYER)
			{
				count_hand(opponent);
				if (hand_size[opponent]) select_card_from_hand(opponent,HUMAN_BROWSE_MODE);
			}
			else
			{
				count_hand(player);
				if (hand_size[player]) select_card_from_hand(player,HUMAN_BROWSE_MODE);
			}
		
			for (int i=0; i<2; i++)
			{
				discard_cards(i,ACTION_CARD);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 11)
	{
		// M. Bison: Evil Charisma
		// Search deck for one Juni and Juli card and
		// add them to your hand. Shuffle your deck
		// Afterwards
		if (combat_ready[player][card_pos] == 0)
		{
			int search = 0;
			search = search_for_card(player,32,CAPCOM_CARD,DECK_PILE_TYPE);
			if (search)
			{
				put_on_top(player,search-1);
				draw_card(player);
			}

			search = search_for_card(player,33,CAPCOM_CARD,DECK_PILE_TYPE);
			if (search)
			{
				put_on_top(player,search-1);
				draw_card(player);
			}
			
			shuffle_deck(player);
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 17)
	{
		// Akuma: Shun Goku Satsu
		// KOs one other character
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_other_chars(player,card_pos);

			if (char_count)
			{
				int selected_char = 0;
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO KO$"};
					while (!selected_char||selected_char == card_pos+1)
					{	
					 	selected_char = select_char(player,select_character,SELECT_ANY);
					}
					
					if (selected_char>10) selected_char -=10; else opponent = player;

					bloodsplat(opponent,selected_char-1);
					put_to_discard_pile(opponent,selected_char-1);
					kill_character(opponent,selected_char-1);
				}
				else
				{
					char select_character [] = {"CHARACTER/DESTROYED$"};

					if (count_chars(opponent))
					{
						int selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
						bloodsplat(opponent,selected_char-1);
						put_to_discard_pile(opponent,selected_char-1);
						kill_character(opponent,selected_char-1);
					}
					else
					{
						int selected_char = select_char(player,select_character,AI_SELECT_WEAK);
						bloodsplat(player,selected_char-1);
						put_to_discard_pile(player,selected_char-1);
						kill_character(player,selected_char-1);
					}
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 19)
	{
		// Chun-Li (A): Sorry!
		// See UP to three cards from opponent's deck
		// Choose 2 cards to discard
		if (combat_ready[player][card_pos] == 0)
		{
			if (player == HUMAN_PLAYER)
			{
				int cards = NUM_CARDS - deck_ptr[opponent];
				int num_discards = 2;
				if (cards<2) num_discards = cards;
				if (cards>3) cards = 3;
				
				while (num_discards)
				{				
					int selected_card = view_top_cards_from_deck(opponent,cards,HUMAN_MODE);
					
					char msg [] = {"CHOOSE ACTION$"};
					char target_001 [] = {"DISCARD$"};
					char target_002 [] = {"NO ACTION$"};
					char *target_menu [] = {target_001,target_002};

					int choice = two_choice_menu2(opponent,msg,target_menu,selected_card-1,DECK_PILE_TYPE);
				
					if (choice == 1)
					{
						put_on_top(opponent,selected_card-1);
						discard_card_from_pile(opponent,1);
						discard_card_animated(opponent);
						num_discards--;
						cards --;
					}
				}
			}
			else
			{
				int card_ai_values[3];
				
				for (int i=0; i<3; i++) card_ai_values[i] = 0;

				int card = deck_ptr[opponent];
				
				for (int i=0; i<3;i++)
				{
					int avg_ai = 0;
					
					if ((card + i)<NUM_CARDS)
					{
						if (card_types[opponent][card+i]!=ACTION_CARD)
						{
							int combat_ai = check_card_in_deck_ai_value(opponent,card+i,AI_COMBAT);
							int deploy_ai = check_card_in_deck_ai_value(opponent,card+i,AI_DEPLOY);
							if (combat_ai>deploy_ai) avg_ai += combat_ai; else avg_ai += deploy_ai;
						}
						else
						{
							avg_ai += check_card_in_deck_ai_value(opponent,card+i,AI_DEPLOY);
						}
					}
					
					card_ai_values[i] = avg_ai;
				}
				
				int num_discards = 2;
				
				int current_ai = 10;
				
				while (current_ai)
				{
					if (num_discards)
					{
						for (int i=0;i<3;i++)
						{
							if (card_ai_values[i]==current_ai)
							{
								if (num_discards)
								{
									put_on_top(opponent,deck_ptr[opponent]+i);
									discard_card_from_pile(opponent,1);
									num_discards--;
								}
							}
						}
					}
					
					current_ai --;
				}
				
				discard_card_animated(opponent);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 21)
	{
		// Rose: Tarot Card
		// Draw 4 cards and discard 3 from hand
		if (combat_ready[player][card_pos]==0)
		{
			draw_cards(player,4);
			count_hand(player);

			int num_discards = 3;
			if (hand_size[player]<3) num_discards = hand_size[player];
			
			if (player == HUMAN_PLAYER)
			{
				while (num_discards)
				{
					int selected_card = select_card_from_hand(player,HUMAN_MODE);
					discard_card(player,selected_card);
					num_discards --;
				}
				
				discard_card_animated(player);
			}
			else
			{
				while (num_discards)
				{
					int selected_card = select_card_from_hand(player,AI_SELECT_WEAK_CARD_MODE);
					discard_card(player,selected_card);
					num_discards --;
				}
				
				discard_card_animated(player);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 26)
	{
		// Dan: What Gives
		// Decreases Enemy SP by 6 Pts
		if (combat_ready[player][card_pos] == 0)
		{
			player_sp[opponent] -=6;
			if (player_sp[opponent]<=0) player_sp[opponent]=0;
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 27)
	{
		// Sakura: Sakura Fight
		// HP doubled if player HP is 1000 or less
		if (combat_ready[player][card_pos] == 0)
		{
			if (player_hp[player]<=1000)
			{
				lightningv(player,card_pos);
				combat_area_card_hp[player][card_pos]*=2;
			}
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 28)
	{
		// Gen: Changing School
		// Return all ACTION cards in discard pile to deck
		// Shuffle deck Afterwards
		
		if (combat_ready[player][card_pos] == 0)
		{
			int ac_cards = count_cards_in_discard_pile(player,ACTION_CARD);
			
			while (ac_cards)
			{
				int selected_card = find_first(player,ACTION_CARD,DISCARD_PILE_TYPE);
				put_to_bottom_of_discard(player,selected_card-1);
				return_to_top_of_deck_from_bottom_of_discard(player);
				ac_cards = count_cards_in_discard_pile(player,ACTION_CARD);
			}
			
			shuffle_deck(player);
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 34)
	{
		// Alex: S. Head Butt
		// Enemy cannot use action cards on his next turn
		action_cards_disabled[opponent] = 255;
		card_effects[player][card_pos] = 0;
	}
	
	if (card_num == 37)
	{
		// Sean: Sore Loser
		// Draw 1 card if opponent has more cards in hand than you
		if (card_effects[player][card_pos] == 2)
		{
			count_hand(player);
			count_hand(opponent);
			if (hand_size[opponent]>hand_size[player])
			{
				if (player == HUMAN_PLAYER) draw_card_animated(player);
				draw_card(player);
			}
			card_effects[player][card_pos] = 0;
		}
	}
	
	if (card_num == 45)
	{
		// Remy: Revenge
		// Players lose SP for each card in hand at the beginning of your turn
		if (card_effects[player][card_pos]==1&&combat_ready[player][card_pos])
		{
			count_hand(player);
			count_hand(opponent);
			player_sp[player] -= hand_size[player];
			player_sp[opponent] -= hand_size[opponent];
			if (player_sp[player]<=0) player_sp[player] = 0;
			if (player_sp[opponent]<=0) player_sp[opponent] = 0;
		}
		
		if (card_effects[player][card_pos] == 2) card_effects[player][card_pos] = 0;
	}
	
	if (card_num == 48)
	{
		// Evil Ryu: Evil Energy
		// KOs 1 character in freeze phase
		if (combat_ready[player][card_pos] == 0)
		{
			int selected_char = 0;
			int num_frozen = 0;
			int enemy_frozen = 0;
			for (int i=0; i<3; i++)
			{
				if (combat_area[player][i]!=255&&freeze_status[player][i]) num_frozen++;

				if (combat_area[opponent][i]!=255&&freeze_status[opponent][i])
				{
					num_frozen++;
					enemy_frozen++;
				}
			}

			if (num_frozen)
			{
				if (player == HUMAN_PLAYER)
				{
					int fstatus = 0;
					char select_character [] = {"SELECT FROZEN/CHARACTER$"};
					while (!selected_char||selected_char == card_pos +1||!fstatus)
					{
						selected_char = select_char(player,select_character,SELECT_ANY);
						if (selected_char>10) fstatus = freeze_status[opponent][selected_char-11]; else fstatus = freeze_status [player][selected_char-1];
					}

					if (selected_char>10) 
					{
							bloodsplat(opponent,selected_char-11);
        						put_to_discard_pile(opponent,selected_char-11);
        						kill_character(opponent,selected_char-11);
					}
					else 
					{
							bloodsplat(player,selected_char-1);
        						put_to_discard_pile(player,selected_char-1);
        						kill_character(player,selected_char-1);
					}
				}
				else
				{
					if (enemy_frozen)
					{
						char select_character [] = {"CHARACTER/DESTROYED$"};
						int max_frozen = select_char(player,select_character,AI_SELECT_ENEMY_FROZEN);
						bloodsplat(opponent,max_frozen-1);
						put_to_discard_pile(opponent,max_frozen - 1);
						kill_character(opponent,max_frozen -1);
					}
					else
					{
						char select_character [] = {"CHARACTER/DESTROYED$"};
						int min_frozen = select_char(player,select_character,AI_SELECT_FROZEN);
						bloodsplat(player,min_frozen-1);
						put_to_discard_pile(player,min_frozen - 1);
        					kill_character(player,min_frozen -1);
					}
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 57)
	{
		// Bishamon: Regretful Lop
		// KO's all other characters in your ring
		// Bishamon's BP is raised by 300 for each KOed character
		if (combat_ready[player][card_pos] == 0)
		{
			for (int i=0; i<3; i++)
			{
				if (i!=card_pos)
				{
					if (combat_area[player][i]!=255)
					{
						bloodsplat(player,i);
						put_to_discard_pile(player,i);
						kill_character(player,i);
						combat_area_card_hp[player][card_pos]+=300;
					}
				}
			}
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 58)
	{
		// Anakaris: J O T P
		// All characters lose abilities
		if (combat_ready[player][card_pos]==0)
		{
			for (int i=0; i<2;i ++)
			{
				circle_abilities_disabled[i] = 1;
				square_abilities_disabled[i] = 1;
			}	
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 61)
	{
		// Pyron: Cosmo Power
		// SP increases by 3 if enemy's SP is higher
		if (combat_ready[player][card_pos])
		{
			if (player_sp[opponent]>player_sp[player]) player_sp[player] += 3;
		}
		
		card_effects[player][card_pos] = 0;
	}

	if (card_num == 66)
	{
		// Q Bee: Plus B
		// Search deck for other Q Bee cards and add them to hand
		// Shuffle your deck Afterwards
		
		if (combat_ready[player][card_pos] == 0)
		{
			int search = 0;
			search = search_for_card(player,66,CAPCOM_CARD,DECK_PILE_TYPE);
			if (search)
			{
				put_on_top(player,search-1);
				draw_card(player);
			}

			search = search_for_card(player,66,CAPCOM_CARD,DECK_PILE_TYPE);
			if (search)
			{
				put_on_top(player,search-1);
				draw_card(player);
			}
			
			shuffle_deck(player);
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 67)
	{
		// Lilith: Becoming One
		// Return all character cards in discard pile to deck
		// Shuffle deck Afterwards
		
		if (combat_ready[player][card_pos] == 0)
		{
			int cards = count_cards_in_discard_pile(player,SNK_CARD);
			
			while (cards)
			{
				int selected_card = find_first(player,SNK_CARD,DISCARD_PILE_TYPE);
				put_to_bottom_of_discard(player,selected_card-1);
				return_to_top_of_deck_from_bottom_of_discard(player);
				cards = count_cards_in_discard_pile(player,SNK_CARD);
			}

			cards = count_cards_in_discard_pile(player,CAPCOM_CARD);
			
			while (cards)
			{
				int selected_card = find_first(player,CAPCOM_CARD,DISCARD_PILE_TYPE);
				put_to_bottom_of_discard(player,selected_card-1);
				return_to_top_of_deck_from_bottom_of_discard(player);
				cards = count_cards_in_discard_pile(player,CAPCOM_CARD);
			}
			
			shuffle_deck(player);
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 68)
	{
		// Hayato: Ashura
		// Discards all action cards. Gain 200 HP for each discarded card

		if (combat_ready[player][card_pos] == 0)
		{
			int ac_cards = count_cards_in_hand(player,ACTION_CARD);

			if (ac_cards) discard_card_animated(player);

			while (ac_cards)
			{
				int selected_card = find_first_in_hand(player,ACTION_CARD);
				discard_card(player,selected_card);
				player_hp[player]+=200;
				ac_cards = count_cards_in_hand(player,ACTION_CARD);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 71)
	{
		// Rain : Back Off
		// Returns 1 other character to your hand
		// or Rain if none are available
		if (combat_ready[player][card_pos]==0)
		{
			int char_count = count_chars(player);
			if (char_count>1)
			{
				int selected_char = 0;
			
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO EXPEL$"};
					while (!selected_char||selected_char==card_pos+1)
					{
						selected_char = select_char(player,select_character,SELECT_OWN);
					}
				}
				else
				{
					char select_character [] = {"CHARACTER/EXPELLED$"};
					selected_char = select_char(player,select_character,AI_SELECT_WEAK);
				}
			
				return_to_hand(player,selected_char-1);
			}
			else
			{
				return_to_hand(player,card_pos);
			}
		}
		
		card_effects[player][card_pos] = 2;	
	}

	if (card_num == 73)
	{
		// BLodia: Energy Cost
		// Uses 10 SP or Blodia is KOed.
		if (combat_ready[player][card_pos] == 0)
		{
			if (player_sp[player]<10)
			{
				bloodsplat(player,card_pos);
				put_to_discard_pile(player,card_pos);
        			kill_character(player,card_pos);
			}
			else
			{
				player_sp[player]-=10;
			}
		}
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 80)
	{
		// Batsu: Boiling Blood
		// All characters in your ring can use square abilities this turn
		square_abilities_disabled[player] = 0;
		card_effects[player][card_pos] = 0;
	}

	if (card_num == 81)
	{
		// Kyosuke: Cool
		// AC card costs are reduced by two
		if (combat_ready[player][card_pos] == 0)
		{
			ac_cost_discounts[player]+=2;
			ac_cost_discounts[opponent]+=2;
		}
		else
		{
			if (!ac_cost_discounts[player]) ac_cost_discounts[player]+=2;
			if (!ac_cost_discounts[opponent]) ac_cost_discounts[opponent]+=2;
		}
		
		card_effects[player][card_pos] = 1;
	}
	
	if (card_num == 82)
	{
		//Hinata: Go For It!
		//Raises 1 character's HP by 200 pts
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = count_other_chars(player,card_pos);
			if (char_count == 0)
			{
				lightningv(player,card_pos);
				combat_area_card_hp[player][card_pos] += 200;
			}
			else
			{
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO POWER UP$"};
					int selected_char = select_char(player,select_character,SELECT_OWN);
					lightningv(player,selected_char-1);
					combat_area_card_hp[player][selected_char-1]+=200;
				}
				else
				{
					char select_character [] = {"CHARACTER/POWERED$"};
					int selected_char = select_char(player,select_character,AI_SELECT_OWN);
					lightningv(player,selected_char-1);
					combat_area_card_hp[player][selected_char-1]+=200;
				}
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 85)
	{
		// Daigo: Finish It
		// HP doubled if enemy has 3 characters
		if (combat_ready[player][card_pos] == 0)
		{
			if (count_chars(opponent) == 3)
			{
				lightningv(player,card_pos);
				combat_area_card_hp[player][card_pos]*=2;
			}
		}
		card_effects[player][card_pos] = 2;
	}

	if (card_num == 86)
	{
		// Kyoko: Massage
		// Ends freeze phase for all characters in your ring
		if (combat_ready[player][card_pos] == 0)
		{
			for (int i=0;i<3;i++)
			{
				if (combat_area[player][i]!=255)
				{
					if (freeze_status[player][i]||attack_status[player][i]||(counter_status[player][i]&&counter_status[player][i]!=255))
					{
						defrost(player,i);
						combat_ready[player][i] = 0;
					}
				}
			}	
			
			clear_freeze_status(player);
			clear_attack_status(player);
		}
		
		card_effects[player][card_pos] = 2;
	}
	
	if (card_num == 89)
	{
		// Ayame: Ohka Gakure
		// Returns to deck. Shuffle Deck Afterwards
		if (card_effects[player][card_pos]==1&&combat_ready[player][card_pos])
		{
			return_to_top(player,card_pos);
			shuffle_deck(player);
		}
		
		if (card_effects[player][card_pos] == 2)
		{
			card_effects[player][card_pos] = 0;
		}
	}

	if (card_num == 118)
	{
		// Saki: Stand by
		// Draw 1 character card from deck. Shuffle Deck and
		// put selcted card on top
		
		if (combat_ready[player][card_pos] == 0)
		{
			int char_count = 0;
			char_count = count_cards_in_deck(player,SNK_CARD);
			char_count += count_cards_in_deck(player,CAPCOM_CARD);
		
			if (char_count)
			{
				int selected_card = 0;
				
				if (player == HUMAN_PLAYER)
				{
					selected_card = select_card_from_deck(player,ANY_CHAR_CARD,HUMAN_MODE);
				}
				else
				{
					selected_card = select_card_from_deck(player,ANY_CHAR_CARD,AI_SELECT_STRONG_MODE);
				}
				
				put_on_top(player,selected_card-1);
				discard_card_from_pile(player,1);
				shuffle_deck(player);
				return_to_top_of_deck_from_bottom_of_discard(player);
			}
		}
		
		card_effects[player][card_pos] = 2;
	}
}

void use_special_abilities_SNK(int player, int card_pos)
{
	int card_num = combat_area[player][card_pos];
	int opponent = 1 - player;
	int used_abilities = 0;
	
	if (card_num == 2)
	{
		// Joe: Hurricane Upper
		// Enemy discards the top 2 cards of his pile
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		discard_card_from_pile(opponent,2);
		used_abilities = 1;
	}
	
	if (card_num == 3)
	{
		//Mai: Morph
		//Discard entire hand. Draw same number of cards plus 1 extra card.
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

		count_hand(player);
		int card_count = hand_size[player]+1;
		
		while (hand_size[player])
        	{
        		discard_card(player,hand_size[player]);
        		count_hand(player);
		}
		
		discard_card_animated(player);
		
		if (player == HUMAN_PLAYER) draw_card_animated(player);
		draw_cards(player,card_count);
		used_abilities = 1;
	}
	
	if (card_num == 7 || card_num == 38)
	{
		int duration = 2;
		//Blue Mary: Arachnid
		//Keeps 1 enemy character in freeze phase until his next turn

		// Rimnerel T: Konril
		// Freezes 1 enemy character until end of your turn

		if (card_num == 38) duration = 1;
		
		int char_count = count_chars(opponent);

		if (char_count)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

			int selected_char = 0;
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT/CHARACTER$"};
				selected_char = select_char(player,select_character,SELECT_ENEMY);
				freeze_status[opponent][selected_char-1] = duration;
			}
			else
			{
				char select_character [] = {"CHARACTER/FROZEN$"};
				selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
				freeze_status[opponent][selected_char-1] = duration;
			}

			used_abilities = 1;
		}
	}
	
	if (card_num == 11)
	{
		//Chong shu: Fogey Fists
		//KOs 1 other character in your ring
		//Add HP according to KOed character's HP

		int char_count = count_chars(player);
		if (char_count>1)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			int selected_char = 0;
			
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT CARD/TO KO$"};
				while(!selected_char||selected_char == card_pos+1)
				{
					selected_char = select_char(player,select_character,SELECT_OWN);
				}
			}
			else
			{
				char select_character [] = {"CHARACTER KOed/CHONG SHU POWERED$"};
				selected_char = select_char(player,select_character,AI_SELECT_WEAK);
			}
			
			int hp = get_hp(player,selected_char-1);
			bloodsplat(player,selected_char-1);
			put_to_discard_pile(player,selected_char-1);
			kill_character(player,selected_char-1);
			lightningv(player,card_pos);
			combat_area_card_hp[player][card_pos] += hp;
			used_abilities = 1;
		}
		
	}

	if (card_num == 19)
	{
		// Ryo: Spirit Surge
		// Select Action Card and Discard it
		// Get 1 SP for each card in hand
		int ac_cards = count_cards_in_hand(player,ACTION_CARD);

		int selected_card = 0;
		int card_data = 0;

		if (ac_cards)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			discard_card_animated(player);
		}
			
		if (ac_cards)
		{
			if (player == HUMAN_PLAYER)
			{
				while (!selected_card)
				{
					selected_card = select_card_from_hand(player,HUMAN_MODE);
					card_data = return_card(player,selected_card);
					if (hand_card_type[player][card_data]!=ACTION_CARD) selected_card = 0;
				}
			}
			else
			{
				selected_card = select_card_from_hand(player,AI_SELECT_WEAK_ACTION_MODE);
			}
			
			discard_card(player,selected_card);
			count_hand(player);
			player_sp[player]+=hand_size[player];				
			used_abilities = 1;
		}
	}
	
	if (card_num == 21)
	{
		// Yuri: Bring it on
		// Lowers enemy's SP by 3
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		player_sp[opponent] -= 3;
		if (player_sp[opponent]<0) player_sp[opponent] = 0;
		used_abilities = 1;
	}

	if (card_num == 23)
	{
		// Mr. Karate: M I A
		// Return Mr Karate to Deck
		// Shuffle Deck
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		return_to_top(player,card_pos);
		shuffle_deck(player);
	}
	
	if (card_num == 29)
	{
		// Kasumi: Overlap Crunch
		// Select 1 Action Card from hand and discard it
		// to deal 300 pts damage to enemy
		
		if (count_cards_in_hand(player,ACTION_CARD))
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

			int selected_card = 0;
			int card_data = 0;

			if (player == HUMAN_PLAYER)
			{
				while (!selected_card)
				{
					selected_card = select_card_from_hand(player,HUMAN_MODE);
					card_data = return_card(player,selected_card);
					if (hand_card_type[player][card_data]!=ACTION_CARD) selected_card = 0;
				}
			}
			else
			{
				selected_card = select_card_from_hand(player,AI_SELECT_WEAK_ACTION_MODE);
			}
			
			discard_card(player,selected_card);
			discard_card_animated(player);

			direct_card_damage(player,card_pos+1,300);
			used_abilities = 1;
		}
	}
	
	if (card_num ==  32)
	{
		// Hanzo: Dust Cloud
		// Return Hanzo to your hand
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		return_to_hand(player,card_pos);
	}
	
	if (card_num == 36)
	{
		// Nakoruru: Mamahaha call
		// Draw 1 card
		if (player == HUMAN_PLAYER) draw_card_animated(player);
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		draw_card(player);
		used_abilities = 1;
	}

	if (card_num == 37)
	{
		// Rimnerel C: Wind Whisper
		// Get 100 Hps
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		player_hp[player]+=100;
		used_abilities = 1;
	}

	if (card_num == 42)
	{
		// Genan: Peeping Kay
		// See opponent's hand
		if (player == HUMAN_PLAYER)
		{
			count_hand(opponent);
			if (hand_size[opponent]) select_card_from_hand(opponent,HUMAN_BROWSE_MODE);
		}
		
		used_abilities = 1;
	}

	if (card_num == 47)
	{
		// Shizumaru: Midsummer Rain
		// KO this card and players discard their hand
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		bloodsplat(player,card_pos);
		put_to_discard_pile(player,card_pos);
        	kill_character(player,card_pos);
        	
        	for (int i=0; i<2; i++)
        	{
        		count_hand(i);	// Init count

			if (hand_size[i]) discard_card_animated(i);

        		while (hand_size[i])
        		{
        			discard_card(i,hand_size[i]);
        			count_hand(i);
        		}
        	}
        	

	}
	
	if (card_num == 52)
	{
		// Shiki C: NRG Syphon
		// KO one of your characters and get 200 HP
		int char_count = count_chars(player);	
		int selected_char = 0;
		
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

		if (char_count == 1)
		{
			selected_char = card_pos+1;
		}	
		else
		{
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT/CHARACTER$"};
				selected_char = select_char(player,select_character,SELECT_OWN);
			}
			else
			{
				char select_character [] = {"CHARACTER/SACRIFICED$"};
				selected_char = select_char(player,select_character,AI_SELECT_WEAK);
			}
		}
			
		used_abilities = 1;
		bloodsplat(player,selected_char-1);
		put_to_discard_pile(player,selected_char-1);
        	kill_character(player,selected_char-1);
		player_hp[player]+= 200;
	}
	
	if (card_num == 61 || card_num == 73)
	{
		int damage = 500;
		// Benimaru: ThunderGod Fist
		// Deals 500 damage to himself and 1 other enemy character

		// Shingo: Burning Shingo
		// Deals 300 damage to himself and 1 other enemy character
		
		if (card_num == 73) damage = 300;
		
		int char_count = count_chars(opponent);	
		int selected_char = 0;
		
		if (char_count)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT ENEMY$"};
				selected_char = select_char(player,select_character,SELECT_ENEMY);
			}
			else
			{
				char select_character [] = {"CHARACTER/DAMAGED$"};
				selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
			}
			
			explode_card(opponent,selected_char-1);
			combat_area_card_hp[opponent][selected_char-1] -= damage;
			if (combat_area_card_hp[opponent][selected_char-1]<=0)
			{
				bloodsplat(opponent,selected_char-1);
				put_to_discard_pile(opponent,selected_char-1);
        			kill_character(opponent,selected_char-1);
			}
		}
		
		explode_card(player,card_pos);
		combat_area_card_hp[player][card_pos] -= damage;
		
		if (combat_area_card_hp[player][card_pos]<=0)
		{
			bloodsplat(player,card_pos);
			put_to_discard_pile(player,card_pos);
        		kill_character(player,card_pos);
		}
		
		used_abilities = 1;
	}

	if (card_num == 67)
	{
		// A Asamiya: Psycho Charge
		// Get 3 SPs
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		player_sp[player]+=3;
		used_abilities = 1;
	}

	if (card_num == 90)
	{
		// Akari P: Demon Night
		// KOs All Characters
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		
		for (int j=0;j<2;j++)
		{
			for (int i=0;i<3;i++)
			{
				if (combat_area[j][i]!=255) bloodsplat(j,i);
				put_to_discard_pile(j,i);
        			kill_character(j,i);
			}
		}
	}

	if (card_num == 100)
	{
		// Kagami: Phoenix
		// KO this character and get 100 HP for each card in hand
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		count_hand(player);
		player_hp[player] += hand_size[player]*100;
		bloodsplat(player,card_pos);
		put_to_discard_pile(player,card_pos);
        	kill_character(player,card_pos);
	}

	if (card_num == 104)
	{
		// Akari: Ponta Leaf
		// Select 1 character card in hand. Akari's HP
		// Becomes selected card's HP
		
		count_hand(player);
		
		if (hand_size[player])
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			int selected_card = 0;
			int card_data = 0;
			
			if (player == HUMAN_PLAYER)
			{
				while (!selected_card)
				{
					selected_card = select_card_from_hand(player,HUMAN_MODE);
					card_data = return_card(player,selected_card);
					if (hand_card_type[player][card_data]!=SNK_CARD&&hand_card_type[player][card_data]!=CAPCOM_CARD) selected_card = 0;
				}
			}
			else
			{
				selected_card = select_card_from_hand(player,AI_SELECT_STRONG_MODE);
			}

			card_data = return_card(player,selected_card);
			
			if (hand_card_type[player][card_data] == SNK_CARD)
			{
				combat_area_card_hp[player][card_pos] = SNK_chars_hp[hand_cards[player][card_data]];
			}
			else
			{
				combat_area_card_hp[player][card_pos] = Capcom_chars_hp[hand_cards[player][card_data]];
			}
			
			discard_card(player,selected_card);
			used_abilities = 1;
		}
	}
	
	if (card_num == 115)
	{
		// Marco: Enemy Chaser
		// Discard hand and All other chracters get 100 Pts damage
		count_hand(player);
		count_hand(opponent);
		
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

		int opp_dmg = 100*hand_size[opponent];
		int own_dmg = 100*hand_size[player];

		for (int i=0; i<2; i++)
		{
			if (hand_size[i]) discard_card_animated(i);
			
        		while (hand_size[i])
        		{
        			discard_card(i,hand_size[i]);
        			count_hand(i);
			}
		}

		for (int i=0;i<3; i++)
		{
			if (combat_area[opponent][i]!=255)
			{
				explode_card(opponent,i);
				combat_area_card_hp[opponent][i] -= opp_dmg;
				if (combat_area_card_hp[opponent][i]<=0)
				{
					bloodsplat(opponent,i);
					put_to_discard_pile(opponent,i);
					kill_character(opponent,i);
				}
			}
				
			if (i!=card_pos&&combat_area[player][i]!=255)
			{
				explode_card(player,i);
				combat_area_card_hp[player][i] -= own_dmg;
				if (combat_area_card_hp[player][i]<=0)
				{
					bloodsplat(player,i);
					put_to_discard_pile(player,i);
					kill_character(player,i);
				}
			}
		}

		used_abilities = 1;
	}
			
	if (used_abilities)
	{
		freeze_status[player][card_pos] = 1;
		attack_status[player][card_pos] = 1;
	}
}

void use_special_abilities_CAPCOM(int player, int card_pos)
{
	int card_num = combat_area[player][card_pos];
	int opponent = 1 - player;
	int used_abilities = 0;
	
	if (card_num == 0||card_num == 18)
	{
		// Ryu: Shinku Hadouken
		// Discard an AC card and give 500 damage to one enemy character

		// Ryu (A): Hadouken
		// Discard an AC card and give 200 damage to one enemy character
		
		if (count_cards_in_hand(player,ACTION_CARD)&&count_chars(opponent))
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

			int damage = 500;
			
			if (card_num == 18) damage = 200;
			
			int selected_card = 0;
			int selected_char = 0;
			int card_data = 0;
			
			if (player == HUMAN_PLAYER)
			{
				while (!selected_card)
				{
					selected_card = select_card_from_hand(player,HUMAN_MODE);
					card_data = return_card(player,selected_card);
					if (hand_card_type[player][card_data]!=ACTION_CARD) selected_card = 0;
				}
			}
			else
			{
				selected_card = select_card_from_hand(player,AI_SELECT_WEAK_ACTION_MODE);
			}
			
			discard_card(player,selected_card);
			discard_card_animated(player);
			
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT ENEMY$"};
				selected_char = select_char(player,select_character,SELECT_ENEMY);
			}
			else
			{
				char select_character [] = {"CHARACTER/DAMAGED$"};
				selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
			}
			
			explode_card(opponent,selected_char-1);
			combat_area_card_hp[opponent][selected_char-1] -= damage;
			
			if (combat_area_card_hp[opponent][selected_char-1]<=0)
			{
				bloodsplat(opponent,selected_char-1);
				put_to_discard_pile(opponent,selected_char-1);
        			kill_character(opponent,selected_char-1);
			}

			used_abilities = 1;
		}
	
	}
	
	if (card_num == 39 || card_num == 63)
	{
		// Elena: Healing
		// Discard an AC card and get 300 HP
		
		// Donovan: Change immortal
		// Discard 1 card from hand and Donovan gains 300 HP
		
		count_hand(player);
		
		if ((count_cards_in_hand(player,ACTION_CARD)&&card_num==39)||(hand_size[player]&&card_num==63))
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

			int selected_card = 0;
			int card_data = 0;
			
			if (player == HUMAN_PLAYER)
			{
				while (!selected_card)
				{
					selected_card = select_card_from_hand(player,HUMAN_MODE);
					card_data = return_card(player,selected_card);
					if (card_num == 39) if (hand_card_type[player][card_data]!=ACTION_CARD) selected_card = 0;
				}
			}
			else
			{
				if (card_num == 39) selected_card = select_card_from_hand(player,AI_SELECT_WEAK_ACTION_MODE);
				if (card_num == 63) selected_card = select_card_from_hand(player,AI_SELECT_WEAK_CARD_MODE);
			}
			
			discard_card(player,selected_card);
			discard_card_animated(player);

			if (card_num == 39) player_hp[player] += 300;
			
			if (card_num == 63)
			{
				lightningv(player,card_pos);
				combat_area_card_hp[player][card_pos] += 300;
			}

			used_abilities = 1;
		}
	}

	if (card_num == 49)
	{
		// Demitri: Midnight Bliss
		// KOs one of your characters. Draw 1 card
		int char_count = count_chars(player);
		
		if (char_count>1)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

			int selected_char = 0;
			
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT CARD/TO KO$"};
				while(!selected_char||selected_char == card_pos+1)
				{
					selected_char = select_char(player,select_character,SELECT_OWN);
				}
			}
			else
			{
				char select_character [] = {"CHARACTER/SACRIFICED$"};
				selected_char = select_char(player,select_character,AI_SELECT_WEAK);
			}
			
			bloodsplat(player,selected_char-1);
			put_to_discard_pile(player,selected_char-1);
        		kill_character(player,selected_char-1);
			draw_card(player);
			
			if (player == HUMAN_PLAYER) draw_card_animated(player);
			
			used_abilities = 1;
		}
	}

	if (card_num == 51)
	{
		// Morrigan P: Goodnight Kiss
		// KOs one of your characters. Get 8 SP
		int char_count = count_chars(player);
		
		if (char_count>1)
		{
			int selected_char = 0;
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT CARD/TO KO$"};
				while(!selected_char||selected_char == card_pos+1)
				{
					selected_char = select_char(player,select_character,SELECT_OWN);
				}
			}
			else
			{
				char select_character [] = {"CHARACTER/SACRIFICED$"};
				selected_char = select_char(player,select_character,AI_SELECT_WEAK);
			}
			
			bloodsplat(player,selected_char-1);
			put_to_discard_pile(player,selected_char-1);
        		kill_character(player,selected_char-1);
			player_sp[player] += 8;			
			used_abilities = 1;
		}
	}

	if (card_num == 65)
	{
		// B B Hood: Apple for you
		// Destroy 1 enemy character. BB Hood returns to deck. Shuffle deck afterwards.
		int char_count = count_chars(opponent);
		if (char_count)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT/CHARACTER$"};
				int selected_char = select_char(player,select_character,SELECT_ENEMY);
						
				bloodsplat(opponent,selected_char-1);
				put_to_discard_pile(opponent,selected_char-1);
				kill_character(opponent,selected_char-1);
			}
			else
			{
				char select_character [] = {"CHARACTER/DESTROYED$"};
				int selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
						
				bloodsplat(opponent,selected_char-1);
				put_to_discard_pile(opponent,selected_char-1);
				kill_character(opponent,selected_char-1);
			}
			
			used_abilities = 1;
			return_to_top(player,card_pos);
			shuffle_deck(player);
		}
	}

	if (card_num == 72)
	{
		// Jin Saotome: Saotome Dynamite
		// KO Jin and 1 other enemy character
		int char_count = count_chars(opponent);
		if (char_count)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			int selected_char = 0;
			if (player == HUMAN_PLAYER)
			{
				char select_character [] = {"SELECT CARD/TO KO$"};
				selected_char = select_char(player,select_character,SELECT_ENEMY);
			}
			else
			{
				char select_character [] = {"CHARACTER/DESTROYED$"};
				selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
			}
			
			bloodsplat(player,card_pos);
			bloodsplat(opponent,selected_char-1);
			put_to_discard_pile(player,card_pos);
			kill_character(player,card_pos);
			put_to_discard_pile(opponent,selected_char-1);
			kill_character(opponent,selected_char-1);
		}
	}
	
	if (card_num == 88)
	{
		// Falcon: Power Stone
		// Randomly discard a card from your hand. Give 1 character 300 HP
		count_hand(player);

		if (hand_size[player])
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			int random_card = (rand()%hand_size[player])+1;
			discard_card(player,random_card);
			discard_card_animated(player);
			
			int char_count = count_chars(player);
			if (char_count == 1)
			{
				lightningv(player,card_pos);
				combat_area_card_hp[player][card_pos] += 300;
			}
			else
			{
				if (player == HUMAN_PLAYER)
				{
					char select_character [] = {"SELECT CARD/TO POWER UP$"};
					int selected_char = select_char(player,select_character,SELECT_OWN);
					lightningv(player,selected_char-1);
					combat_area_card_hp[player][selected_char-1]+=300;
				}
				else
				{
					char select_character [] = {"CHARACTER/POWERED$"};
					int selected_char = select_char(player,select_character,AI_SELECT_OWN);
					lightningv(player,selected_char-1);
					combat_area_card_hp[player][selected_char-1]+=300;
				}
			}
			
			used_abilities = 1;
		}
	}

	if (card_num == 93)
	{
		// Hiryu: Cypher
		// KOs 1 character with backup
		int selected_char = 0;
		int num_w_backups = 0;
		int enemy_w_backups = 0;
		
		for (int i=0; i<3; i++)
		{
			if (combat_area[player][i]!=255&&num_backups[player][i]) num_w_backups++;

			if (combat_area[opponent][i]!=255&&num_backups[opponent][i])
			{
				num_w_backups++;
				enemy_w_backups++;
			}
		}

		if (num_w_backups)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			if (player == HUMAN_PLAYER)
			{
				int bstatus = 0;
				char select_character [] = {"SELECT CARD/WTIH BACKUP$"};
				while (!selected_char||selected_char == card_pos +1||!bstatus)
				{
					selected_char = select_char(player,select_character,SELECT_ANY);
					if (selected_char>10) bstatus = num_backups[opponent][selected_char-11]; else bstatus = num_backups[player][selected_char-1];
				}
	
				if (selected_char>10) 
				{
					bloodsplat(opponent,selected_char-11);
        				put_to_discard_pile(opponent,selected_char-11);
        				kill_character(opponent,selected_char-11);
				}
				else 
				{
					bloodsplat(player,selected_char-1);
        				put_to_discard_pile(player,selected_char-1);
        				kill_character(player,selected_char-1);
				}
			}
			else
			{
				if (enemy_w_backups)
				{
					char select_character [] = {"CHARACTER/DESTROYED$"};
					int max_backup = select_char(player,select_character,AI_SELECT_ENEMY_WITH_BACKUPS);
					bloodsplat(opponent,max_backup-1);
					put_to_discard_pile(opponent,max_backup - 1);
					kill_character(opponent,max_backup -1);
				}
				else
				{
					char select_character [] = {"CHARACTER/DESTROYED$"};
					int min_backups = select_char(player,select_character,AI_SELECT_BACKUPS);
					bloodsplat(player,min_backups - 1);
					put_to_discard_pile(player,min_backups - 1);
        				kill_character(player,min_backups -1);
				}
			}
			
			used_abilities = 1;
		}
	}

	if (card_num == 96)
	{
		// Haggar: Spinning Lariat
		// All characters return to deck
		// Shuffle Deck afterwards
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		
		for (int j=0; j<2; j++)
		{
			for (int i=0; i<3; i++)
			{
				if (combat_area[j][i]!=255)
				{
					return_to_top(j,i);
				}
			}
			
			shuffle_deck(j);
		}
	}
	
	if (card_num == 97)
	{
		// DAMND: Whistle
		// Draws 1 character randomly from deck and add it
		// to your hand
		
		int num_chars = 0;
		num_chars = count_cards_in_deck(player,SNK_CARD);
		num_chars += count_cards_in_deck(player,CAPCOM_CARD);
		
		if (num_chars)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			int random_char = rand()%num_chars+1;
			put_on_top(player,return_char_in_deck(player,random_char));
			draw_card(player);
			if (player == HUMAN_PLAYER) draw_card_animated(player);
			used_abilities = 1;
		}
	}
	
	if (card_num == 98)
	{
		// C. Commando:  C. Corridor
		// All characters in freeze phase receive 400 pts damage
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		
		freeze_status[player][card_pos] = 1;
		
		for (int j=0; j<2; j++)
		{
			for (int i=0; i<3; i++)
			{
				if (combat_area[j][i]!=255&&freeze_status[j][i])
				{
					explode_card(j,i);
					combat_area_card_hp[j][i] -= 400;
					if (combat_area_card_hp[j][i]<=0)
					{
						bloodsplat(j,i);
						put_to_discard_pile(j,i);
						kill_character(j,i);
					}
				}
			}
		}
		
		used_abilities = 1;
	}	

	if (card_num == 102)
	{
		// Leon: Confinement
		// Enemy cannot use action cards on his next turn
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
		action_cards_disabled[opponent] = 2;
		used_abilities = 1;
	}

	if (card_num == 105)
	{
		// Jill: Beretta
		// Decreases Enemy Character or Player HP by 100

		int char_count = count_chars(opponent);
		if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);

		if (char_count)
		{
			if (player == HUMAN_PLAYER)
			{
				char msg [] = {"SELECT TARGET$"};
				char target_001 [] = {"PLAYER$"};
				char target_002 [] = {"CHARACTER$"};
				char *target_menu [] = {target_001,target_002};

				int choice = two_choice_menu(player,card_pos,msg,target_menu);
					
				if (choice == 1)
				{
					direct_card_damage(player,card_pos+1,100);
				}
				else
				{
					char select_character [] = {"SELECT/CHARACTER$"};
					int selected_char = select_char(player,select_character,SELECT_ENEMY);
						
					explode_card(opponent,selected_char-1);
					combat_area_card_hp[opponent][selected_char-1]-=300;
					if (combat_area_card_hp[opponent][selected_char-1]<=0)
					{
						bloodsplat(opponent,selected_char-1);
						put_to_discard_pile(opponent,selected_char-1);
						kill_character(opponent,selected_char-1);
					}
				}
			}
			else
			{
				if (count_chars(COMPUTER_PLAYER)>count_chars(HUMAN_PLAYER))
				{
					direct_card_damage(player,card_pos+1,100);
				}
				else
				{
					char select_character [] = {"CHARACTER/DAMAGED$"};
					int selected_char = select_char(player,select_character,AI_SELECT_ENEMY_STRONG);
						
					explode_card(opponent,selected_char-1);
					combat_area_card_hp[opponent][selected_char-1]-=100;
					if (combat_area_card_hp[opponent][selected_char-1]<=0)
					{
						bloodsplat(opponent,selected_char-1);
						put_to_discard_pile(opponent,selected_char-1);
						kill_character(opponent,selected_char-1);
					}
				}
			}
		}
		else
		{
			direct_card_damage(player,card_pos+1,100);
		}
		
		used_abilities = 1;
	}

	if (card_num == 107)
	{
		// Roll:  C. Roll Buster
		// All characters receive 200 pts damage
		
		if (count_chars(player)||count_chars(opponent))
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			for (int j=0; j<2; j++)
			{
				for (int i=0; i<3; i++)
				{
					if (combat_area[j][i]!=255)
					{
						explode_card(j,i);
						combat_area_card_hp[j][i] -= 200;
						if (combat_area_card_hp[j][i]<=0)
						{
							bloodsplat(j,i);
							put_to_discard_pile(j,i);
							kill_character(j,i);
						}
					}
				}
			}
		
			used_abilities = 1;
		}
	}	

	if (card_num == 110)
	{
		// Megaman X: Parts Buster
		// Discard an AC card and Draw 2 cards
		if (count_cards_in_hand(player,ACTION_CARD))
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			int selected_card = 0;
			int card_data = 0;
			
			if (player == HUMAN_PLAYER)
			{
				while (!selected_card)
				{
					selected_card = select_card_from_hand(player,HUMAN_MODE);
					card_data = return_card(player,selected_card);
					if (hand_card_type[player][card_data]!=ACTION_CARD) selected_card = 0;
				}
			}
			else
			{
				selected_card = select_card_from_hand(player,AI_SELECT_WEAK_ACTION_MODE);
			}
			
			discard_card(player,selected_card);
			discard_card_animated(player);
			draw_cards(player,2);
						
			used_abilities = 1;
		}
	}

	if (card_num == 115)
	{
		// Ryu B:  Transform
		// All enemy character receive 100 pts damage
		int char_count = count_chars(opponent);
		if (char_count)
		{
			if (player == COMPUTER_PLAYER) computer_deploys_card(uses_abilities,card_pos+1);
			for (int i=0; i<3; i++)
			{
				if (combat_area[opponent][i]!=255)
				{
					explode_card(opponent,i);
					combat_area_card_hp[opponent][i] -= 100;
					if (combat_area_card_hp[opponent][i]<=0)
					{
						bloodsplat(opponent,i);
						put_to_discard_pile(opponent,i);
						kill_character(opponent,i);
					}
				}
			}
		
			used_abilities = 1;
		}
	}	
	
	if (used_abilities)
	{
		freeze_status[player][card_pos] = 1;
		attack_status[player][card_pos] = 1;
	}
}

void use_special_abilities(int player, int card_pos)
{
	if (square_abilities_disabled[player]) return;
	if (combat_area[player][card_pos]==255) return;
	if (combat_ready[player][card_pos]!=1) return;
	if (attack_status[player][card_pos]||freeze_status[player][card_pos]||counter_status[player][card_pos]) return;

	int card_type = combat_area_card_type[player][card_pos];
	
	if (card_type == SNK_CARD) use_special_abilities_SNK(player,card_pos);
	if (card_type == CAPCOM_CARD) use_special_abilities_CAPCOM(player,card_pos);
}


// handles special card abilities (card enters play or end of turn effects)
void do_special_effects(int player, int card_pos)
{
// performs special effects as cards enter or leave play
	if (combat_area[player][card_pos]==255) return;
	if (circle_abilities_disabled[player]) return;
	
	int card_type = combat_area_card_type[player][card_pos];
	char *char_ability;
	
	if (card_type == SNK_CARD)
	{
		char_ability = (char *)SNK_ABILITIES[combat_area[player][card_pos]];
		int ability = ability_stats(char_ability);
		if (ability==STATS_TRIANGLE&&triangle_abilities_disabled[player])
		{
			card_effects[player][card_pos] = 2;	
		}
		
		do_special_effects_SNK(player,card_pos);
	}
	else
	{
		char_ability = (char *)CAPCOM_ABILITIES[combat_area[player][card_pos]];
		int ability = ability_stats(char_ability);
		if (ability==STATS_TRIANGLE&&triangle_abilities_disabled[player])
		{
			card_effects[player][card_pos] = 2;	
		}
		
		do_special_effects_Capcom(player,card_pos);		
	}
}

void do_end_of_turn_effects(int player)
{
// performs end of turn effects
// calls do_special_effects
	if (circle_abilities_disabled[player]) return;
	
	for (int i=0; i<3; i++)
	{
		do_special_effects(player,i);
	}
}

void do_start_of_turn_effects(int player)
{
	if (circle_abilities_disabled[player]) return;
	
	for (int i=0; i<3; i++)
	{
		if (combat_area[player][i]!=255&&combat_ready[player][i]&&card_effects[player][i]==0) do_special_effects(player,i);
	}	
}

void use_special_card(int player, int card)
{
	if (action_cards_disabled[player]) return;
}

int can_block(int player, int pos)
{
    if (!circle_abilities_disabled[player])
    {
    	if (combat_area[player][pos]!=255&&!attack_status[player][pos]&&!counter_status[player][pos]&&!freeze_status[player][pos]) return 1; else return 0;
    }
    else
    {
    	if (combat_area[player][pos]!=255&&!attack_status[player][pos]&&(counter_status[player][pos]==255)&&!freeze_status[player][pos]) return 1;
    	if (combat_area[player][pos]!=255&&(attack_status[player][pos]==255)&&(counter_status[player][pos]==255)&&!freeze_status[player][pos]) return 1;
    	if (combat_area[player][pos]!=255&&(attack_status[player][pos]==255)&&!counter_status[player][pos]&&!freeze_status[player][pos]) return 1;
    	if (combat_area[player][pos]!=255&&!attack_status[player][pos]&&!counter_status[player][pos]&&!freeze_status[player][pos]) return 1;
    	return 0;
    }
}

int can_attack(int player, int pos)
{
    if (!circle_abilities_disabled[player])
    {
    	if (combat_area[player][pos]!=255&&!attack_status[player][pos]&&!freeze_status[player][pos]&&combat_ready[player][pos]==1) return 1; else return 0;
    }
    else
    {
    	if (combat_area[player][pos]!=255&&!attack_status[player][pos]&&!freeze_status[player][pos]&&combat_ready[player][pos]==1) return 1;
    	if (combat_area[player][pos]!=255&&(attack_status[player][pos]==255)&&!freeze_status[player][pos]&&combat_ready[player][pos]==1) return 1;
    	return 0;
    }
}

int check_req_SNK(int card)
{
	int char_num = hand_cards[COMPUTER_PLAYER][card];
	
	if (char_num == 64 || char_num == 84) return count_chars(HUMAN_PLAYER);

	if (char_num == 24)
	{
		count_hand(COMPUTER_PLAYER);
		count_hand(HUMAN_PLAYER);
		if (!(hand_size[COMPUTER_PLAYER]<2)||(hand_size[HUMAN_PLAYER]-hand_size[COMPUTER_PLAYER]>2)) return 0;
	}
	
	if (char_num == 31 || char_num == 45 || char_num == 75)
	{
		if (count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)&&player_hp[COMPUTER_PLAYER]<500) return 0;
	}
	
	if (char_num == 48)
	{
		int enemy_frozen = 0;
		int ally_frozen = 0;
		
		for (int i=0; i<3; i++)
		{
			if (freeze_status[HUMAN_PLAYER]&&combat_area[HUMAN_PLAYER][i]!=255) enemy_frozen++;
			if (freeze_status[COMPUTER_PLAYER]&&combat_area[COMPUTER_PLAYER][i]!=255) ally_frozen++;
		}
		
		if (ally_frozen<enemy_frozen) return 0;
	}
	
	if (char_num == 56)
	{
		int max_hp = 0;
		int amak_hp = SNK_chars_hp[56];
		for (int i=0; i<3; i++)
		{
			if (combat_area[COMPUTER_PLAYER][i]!=255)
			{
				int char_hp = get_hp(COMPUTER_PLAYER,i);
				if (char_hp>max_hp) max_hp = char_hp;
			}
		}
		
		int enemy_count = count_chars(HUMAN_PLAYER);
		int char_count = count_chars(COMPUTER_PLAYER);

		if (!enemy_count)
		{
			if (!char_count) return 1;
			if (char_count == 1)
			{
				if (amak_hp>max_hp) return 1; 
			}
			else
			{
				if (amak_hp>max_hp&&player_hp[COMPUTER_PLAYER]>500) return 1;
			}
			
			return 0;
		}
		
		if (char_count)
		{
			if (char_count == 1)
			{
				if (amak_hp>max_hp) return 1;
			}
			else
			{
				if (enemy_count <= char_count && enemy_count != 3)
				{
					if (amak_hp>max_hp&&player_hp[COMPUTER_PLAYER]>500) return 1;
				}

				return 0;				
			}
		}
	}
		
	if (char_num == 58) return 0;
	
	if (char_num == 83)
	{
		int enemy_count = count_chars(HUMAN_PLAYER);
		int char_count = count_chars(COMPUTER_PLAYER);
		int max_hp[2];
		max_hp[0] = 0;
		max_hp[1] = 0;
		
		for (int j=0; j<2; j++)
		{	for (int i=0; i<3; i++)
			{
				int char_hp = get_hp(j,i);
				if (char_hp>max_hp[j]) max_hp[j] = char_hp;
			}
		}
		
		if (char_count>=enemy_count)
		{
			if (max_hp[HUMAN_PLAYER]<800)
			{
				if (max_hp[COMPUTER_PLAYER]>1000||max_hp[COMPUTER_PLAYER]<800) return 0;
			}
			
			return 1;
		}
		else
		{
			if (max_hp[HUMAN_PLAYER]>=800) return 1;
			return 0;
		}
		
		return 0;
	}

	if (char_num == 85)
	{
		int char_count = count_chars(COMPUTER_PLAYER);
		int max_hp[2];
		int min_hp[2];
		max_hp[0] = 0;
		max_hp[1] = 0;
		min_hp[0] = 0;
		min_hp[1] = 0;

		for (int j=0; j<2; j++)
		{	for (int i=0; i<3; i++)
			{
				int char_hp = get_hp(j,i);
				if (char_hp>max_hp[j]) max_hp[j] = char_hp;
				if (char_hp<min_hp[j]||!min_hp[j]) min_hp[j]= char_hp;
			}
		}
		
		if (!char_count) return 1;
		if (min_hp[HUMAN_PLAYER]<=300||max_hp[HUMAN_PLAYER]<=300) return 1;
		if (min_hp[COMPUTER_PLAYER]>300) return 1;
		
		return 0;
	}

	if (char_num == 102)
	{
		count_hand(COMPUTER_PLAYER);
		
		int min_hp = 0;
		
		for (int i=0; i<hand_size[COMPUTER_PLAYER]; i++)
		{
			int card = return_card(COMPUTER_PLAYER,i+1);
			if (hand_card_type[COMPUTER_PLAYER][card]!=ACTION_CARD)
			{
				if (hand_card_type[COMPUTER_PLAYER][card]==SNK_CARD)
				{
					int hp = SNK_chars_hp[hand_cards[COMPUTER_PLAYER][card]];
					if (hp<min_hp||!min_hp) min_hp = hp;
				}
				else
				{
					int hp = Capcom_chars_hp[hand_cards[COMPUTER_PLAYER][card]];
					if (hp<min_hp||!min_hp) min_hp = hp;
				}
			}
		}
		
		if (min_hp>600) return 0; 
	}
	
	return 1;
}

int check_req_CAPCOM(int card)
{	
	int char_num = hand_cards[COMPUTER_PLAYER][card];
	
	if (char_num == 2)
	{
		if (count_chars(COMPUTER_PLAYER)>count_chars(HUMAN_PLAYER)) return 0;
	}

	if (char_num == 17) return count_chars(HUMAN_PLAYER);

	if (char_num == 45)
	{
		count_hand(HUMAN_PLAYER);
		count_hand(COMPUTER_PLAYER);
		
		if (hand_size[COMPUTER_PLAYER]>hand_size[HUMAN_PLAYER]) return 0;
	}
	
	if (char_num == 45)
	{
		count_hand(HUMAN_PLAYER);
		count_hand(COMPUTER_PLAYER);
		
		if (hand_size[COMPUTER_PLAYER]>hand_size[HUMAN_PLAYER]) return 0;
	}

	if (char_num == 48)
	{
		int enemy_frozen = 0;
		int ally_frozen = 0;
		
		for (int i=0; i<3; i++)
		{
			if (freeze_status[HUMAN_PLAYER]&&combat_area[HUMAN_PLAYER][i]!=255) enemy_frozen++;
			if (freeze_status[COMPUTER_PLAYER]&&combat_area[COMPUTER_PLAYER][i]!=255) ally_frozen++;
		}
		
		if (!enemy_frozen&&ally_frozen) return 0;
	}
	
	if (char_num == 57)
	{
		if (count_chars(HUMAN_PLAYER)>1&&count_chars(COMPUTER_PLAYER)>1) return 0;
	}
	

	if (char_num == 71)
	{
		if (!count_chars(COMPUTER_PLAYER)) return 0;
		
		int found_weak = 0;
		
		for (int i=0; i<3;i++)
		{
			if (combat_area[COMPUTER_PLAYER][i]!=255)
			{
				if (get_hp(COMPUTER_PLAYER,i)<600) found_weak++;
			}
		}
		
		if (!found_weak) return 0;
	}

	if (char_num == 73)
	{
		if (player_sp[COMPUTER_PLAYER]<10) return 0;
	}

	if (char_num == 83) return 0;
	
	return 1;
}

// For cards with abilities, this subroutine checks the
// requirements, i.e. card can be deployed without problems
int check_req(int card_num)
{
	if (card_num == 0) return 0;
	
	int card = return_card(COMPUTER_PLAYER,card_num);

	if (hand_card_type[COMPUTER_PLAYER][card] == SNK_CARD) return check_req_SNK(card);
	if (hand_card_type[COMPUTER_PLAYER][card] == CAPCOM_CARD) return check_req_CAPCOM(card);

	return 0;
}

int select_best(int ai_value, int ai_type,int num_matches)
{
	if (num_matches == 0) return 0;
/*
	if (num_matches == 1)
	{
		int card_to_use = 0;
		count_hand(COMPUTER_PLAYER);
		for (int i=0; i<hand_size[COMPUTER_PLAYER]; i++)
		{
			int card = return_card(COMPUTER_PLAYER,i+1);
			if (hand_card_type[COMPUTER_PLAYER][card]!=ACTION_CARD)
			{
				if (check_card_ai_value(COMPUTER_PLAYER,i+1,ai_type)==ai_value)
				{
					if (check_req(i+1)) card_to_use = i+1;
				}
			}
			
		}
		
		return card_to_use;
	}
*/	
	int num_meets_req = 0;
	int num_chars = 0;
	//int max_sp = 0;
	int max_hp = 0;
	int max_hp_card = 0;
        //int max_sp_card = 0;
	
	int first_card = 0;
	
	num_chars = count_chars(COMPUTER_PLAYER);
	
	if (!deploy_status[COMPUTER_PLAYER]&&num_chars<3)
	{
		count_hand(COMPUTER_PLAYER);
		
		for (int i=0; i<hand_size[COMPUTER_PLAYER]; i++)
		{
			int card = return_card(COMPUTER_PLAYER,i+1);
			if (hand_card_type[COMPUTER_PLAYER][card]!=ACTION_CARD)
			{
				if (check_card_ai_value(COMPUTER_PLAYER,i+1,ai_type)==ai_value)
				{
					if (check_req(i+1))
					{
						num_meets_req++;
						if (num_meets_req == 1) first_card = i+1;
					}
				}
			}
		}
		
		if (!num_meets_req) return 0;
		if (num_meets_req == 1) return first_card;

		count_hand(COMPUTER_PLAYER);
		for (int i=0; i<hand_size[COMPUTER_PLAYER]; i++)
		{
			int card = return_card(COMPUTER_PLAYER,i+1);
			if (hand_card_type[COMPUTER_PLAYER][card]!=ACTION_CARD)
			{
				if (check_card_ai_value(COMPUTER_PLAYER,i+1,ai_type)==ai_value)
				{
					if (check_req(i+1))
					{
                         			if (hand_card_type[COMPUTER_PLAYER][card] == SNK_CARD)
                         			{
                              				int char_hp = SNK_chars_hp[hand_cards[COMPUTER_PLAYER][card]];
                              				if (char_hp >= max_hp)
                              				{
                                   				max_hp = char_hp;
                                   				max_hp_card = i+1;
                              				}

                              				//int char_sp = SNK_chars_sp[hand_cards[COMPUTER_PLAYER][i]];
                              				//if (char_sp >= max_sp)
                              				//{
                                   			//	max_sp = char_sp;
                                   			//	max_sp_card = card;
                              				//}
                         			}
                         			else
                         			{
                              				int char_hp = Capcom_chars_hp[hand_cards[COMPUTER_PLAYER][i]];
                              				if (char_hp >= max_hp)
                              				{
                                   				max_hp = char_hp;
                                   				max_hp_card = i+1;
                              				}

                              				//int char_sp = Capcom_chars_sp[hand_cards[COMPUTER_PLAYER][i]];
                              				//if (char_sp >= max_sp)
                              				//{
                                   			//	max_sp = char_sp;
                                   			//	max_sp_card = card;
                              				//}
                         			}
					}
				}
			}
		}
		
		//if (num_chars<2)
		//{
			//return max_hp_card;
		//}
		//else
		//{
                //     	if (player_sp[COMPUTER_PLAYER]<5&&player_hp[COMPUTER_PLAYER]>500) return max_sp_card; else return max_hp_card;
		//}
		
		return max_hp_card;
	}
	
	return 0;
}

void do_computer_selects_char_to_deploy()
{
	computer_deployed_card = 0;
	int current_ai = 10;
	int current_ai_type = 0;
	int first_match = 0;
	int found_card = 0;

	if (count_chars(COMPUTER_PLAYER)==3) return;
	
	count_hand(COMPUTER_PLAYER);
		
	while (!found_card&&current_ai)
	{
		int found_ai_matches = 0;
		int ai_type = 0;
		int high_ai_value_found = 0;

		current_ai_type = 0;
					
		while (!high_ai_value_found&&(ai_type<3))
		{
			for (int i=0; i<hand_size[COMPUTER_PLAYER]; i++)
			{
				int card = return_card(COMPUTER_PLAYER,i+1);
			
				if (hand_card_type[COMPUTER_PLAYER][card]!=ACTION_CARD)
				{
					if (check_card_ai_value(COMPUTER_PLAYER,i+1,ai_type)==current_ai)
					{
						found_ai_matches++;
						if (found_ai_matches == 1) first_match = i+1;
					}
				}
			}
			
			if (found_ai_matches) high_ai_value_found = 1;

			if (!high_ai_value_found) 
			{
				if (ai_type<3) ai_type++;
			}

			current_ai_type = ai_type;
					
			if (high_ai_value_found) break;
		}

		if (found_ai_matches == 1)
		{
			if (check_req(first_match)) found_card = first_match;
		}
		else
		{
			if (high_ai_value_found) found_card = select_best(current_ai,current_ai_type,found_ai_matches);
		}
		
		if (!found_card)
		{
			current_ai--;
			first_match = 0;
		}
		
		if (found_card) break;
	}
	
	if (found_card)
	{
		int card_deployed = 0;
                int deploy_pos = 0;
                     
                for (int i=0; i<3; i++)
                {
			if (!card_deployed&&combat_area[COMPUTER_PLAYER][i] == 255)
                        {
                        	card_deployed = 1;
                        	deploy_pos = i;
                        }
		}
                     
                if (card_deployed)
                {
                	int card = return_card(COMPUTER_PLAYER,found_card);
                	place_card(deploy_pos,COMPUTER_PLAYER,card);
                	computer_deployed_card = deploy_pos + 1;
                	set_deploy_status(COMPUTER_PLAYER);
		}
		else
		{
			computer_deployed_card = 0;
		}
	}
}

int check_abilities_req_SNK(int card_pos)
{
	int char_num = combat_area[COMPUTER_PLAYER][card_pos];
	
	if (char_num == 3)
	{
		count_hand(COMPUTER_PLAYER);
		if (hand_size[COMPUTER_PLAYER]>1) return 0;
		if (deploy_status[COMPUTER_PLAYER]) return 0;
	}
	
	if (char_num == 7 || char_num == 38)
	{
		if (!count_chars(HUMAN_PLAYER)) return 0;
		if (count_chars(COMPUTER_PLAYER)==2&&deploy_status[COMPUTER_PLAYER]) return 0;
	}
	
	if (char_num == 11)
	{
		if (count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)) return 0;
		if ((count_chars(COMPUTER_PLAYER)-count_chars(HUMAN_PLAYER))<2)
		{
			if (deploy_status[COMPUTER_PLAYER]) return 0;
		}
	}

	if (char_num == 19)
	{
		count_hand(COMPUTER_PLAYER);
		if (hand_size[COMPUTER_PLAYER]<5) return 0;
		if (player_sp[COMPUTER_PLAYER]>5) return 0;	
	}
	
	if (char_num == 21 || char_num == 37)
	{
		if ((count_chars(COMPUTER_PLAYER)-count_chars(HUMAN_PLAYER))<2)
		{
			if (deploy_status[COMPUTER_PLAYER]) return 0;
		}
		
		if (player_sp[HUMAN_PLAYER]<3) return 0;
	}
	
	if (char_num == 23||char_num==32)
	{
		if (combat_area_card_hp[COMPUTER_PLAYER][card_pos]>200) return 0;
		if (deploy_status[COMPUTER_PLAYER]) return 0;
	}

	if (char_num == 29||char_num == 36||char_num==37||char_num==38||char_num==67||char_num==74)
	{
		if (!count_chars(HUMAN_PLAYER)||count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)) return 0;
	}
	
	if (char_num == 42) return 0;
	
	if (char_num == 47)
	{
		if ((count_chars(COMPUTER_PLAYER)-count_chars(HUMAN_PLAYER))<2) return 0;
		count_hand(HUMAN_PLAYER);
		count_hand(COMPUTER_PLAYER);
		if (hand_size[COMPUTER_PLAYER]>hand_size[HUMAN_PLAYER]) return 0;
		if (hand_size[COMPUTER_PLAYER]>2) return 0;
	}
	
	if (char_num == 52)
	{
		if ((count_chars(COMPUTER_PLAYER)-count_chars(HUMAN_PLAYER))<2) return 0;

		int found_char = 0;
		
		for (int i=0; i<3; i++)
		{
			if (!found_char)
			{
				if (combat_area[COMPUTER_PLAYER][i]!=255&&combat_area_card_hp[COMPUTER_PLAYER][i]<=200)
				{
					if (!deploy_status[COMPUTER_PLAYER]) found_char = i+1;
				}
			}
		}
		
		if (!found_char) return 0;
		if (check_char_ai_value(found_char-1,AI_ABILITY)>check_char_ai_value(card_pos,AI_ABILITY)) return 0;
	}
	
	if (char_num == 61 || char_num == 73)
	{
		int damage = 500;	
		if (char_num == 73) damage = 300;
		
		if (get_hp(COMPUTER_PLAYER,card_pos)<damage)
		{
			if (!count_chars(HUMAN_PLAYER)) return 0;
			int can_be_killed = 0;
			
			for (int i=0; i<3; i++)
			{
				if (combat_area[HUMAN_PLAYER][i]!=255)
				{
					if (get_hp(HUMAN_PLAYER,i)<=damage)
					{
						if (!can_be_killed) can_be_killed = 1;
					}
				}
			}
			
			if (!can_be_killed) return 0;
		}
		else
		{
			if (count_chars(COMPUTER_PLAYER)<count_chars(HUMAN_PLAYER)) return 0;
		}
	}
	
	if (char_num == 90)
	{
		if (count_chars(COMPUTER_PLAYER)>count_chars(HUMAN_PLAYER)) return 0;
		if (!deploy_status[COMPUTER_PLAYER]) return 0;
	}
	
	if (char_num == 91||char_num==100)
	{
		if (count_chars(COMPUTER_PLAYER)<count_chars(HUMAN_PLAYER)) return 0;
		if (get_hp(COMPUTER_PLAYER,card_pos)>200) return 0;
		count_hand(COMPUTER_PLAYER);
		if (hand_size[COMPUTER_PLAYER]<2) return 0;
	}

	if (char_num == 104)
	{
		if (count_chars(COMPUTER_PLAYER)<count_chars(HUMAN_PLAYER)) return 0;
		if (!count_chars(HUMAN_PLAYER)) return 0;
		count_hand(COMPUTER_PLAYER);
		int max_hp = 0;
		for (int i=0;i<hand_size[COMPUTER_PLAYER];i++)
		{
			int card_num = return_card(COMPUTER_PLAYER,i+1);
			if (hand_card_type[COMPUTER_PLAYER][card_num]!=ACTION_CARD)
			{
				int hp = 0;
				
				if (hand_card_type[COMPUTER_PLAYER][card_num]==SNK_CARD)
				{
					hp = SNK_chars_hp[hand_cards[COMPUTER_PLAYER][card_num]];
				}
				else
				{
					hp = Capcom_chars_hp[hand_cards[COMPUTER_PLAYER][card_num]];
				}
				
				if (hp>max_hp||!max_hp) max_hp = hp;
			}
		}
		
		if (max_hp<get_hp(COMPUTER_PLAYER,card_pos)) return 0;
	}

	if (char_num == 115) return 0;
	
	return 1;	
}

int check_abilities_req_CAPCOM(int card_pos)
{
	int char_num = combat_area[COMPUTER_PLAYER][card_pos];
	
	if (char_num == 0||char_num==18||char_num==39|char_num==55||char_num==63||char_num==77)
	{
		if (!count_chars(HUMAN_PLAYER)) return 0;
		if (count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)) return 0;
	}

	if (char_num == 49||char_num==51)
	{
		if (deploy_status[COMPUTER_PLAYER]) return 0;
		if (count_chars(COMPUTER_PLAYER)>count_chars(HUMAN_PLAYER))
		{
			int weak_char = 0;
			for (int i=0; i<3; i++)
			{
				if (card_pos!=i&&combat_area[COMPUTER_PLAYER][i]!=255)
				{
					if (get_hp(COMPUTER_PLAYER,i)<get_hp(COMPUTER_PLAYER,card_pos))
					{
						weak_char = i+1;
					}
				}
			}
			
			if (weak_char) return 1;
		}
		
		return 0;
	}

	if (char_num == 65||char_num==96)
	{
		if (get_hp(COMPUTER_PLAYER,card_pos)>200) return 0;
		if (count_chars(COMPUTER_PLAYER)>count_chars(HUMAN_PLAYER)) return 0;
		return 1;
	}

	if (char_num == 72)
	{
		if (!count_chars(HUMAN_PLAYER)) return 0;

		int max_enemy_hp = 0;
		for (int i=0; i<3; i++)
		{
			if (combat_area[HUMAN_PLAYER][i]!=255&&get_hp(HUMAN_PLAYER,i)>max_enemy_hp) max_enemy_hp = get_hp(HUMAN_PLAYER,i);
		}
			
		if (max_enemy_hp<get_hp(COMPUTER_PLAYER,card_pos)) return 0;
	}

	if (char_num==88||char_num==94||char_num==97||char_num==102||char_num==103)
	{
		if (!count_chars(HUMAN_PLAYER)) return 0;
		if (count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)) return 0;
	}

	if (char_num == 98)
	{
		int enemy_frozen = 0;
		int ally_frozen = 0;
		
		for (int i=0; i<3; i++)
		{
			if (freeze_status[HUMAN_PLAYER]&&combat_area[HUMAN_PLAYER][i]!=255) enemy_frozen++;
			if (freeze_status[COMPUTER_PLAYER]&&combat_area[COMPUTER_PLAYER][i]!=255) ally_frozen++;
		}
		
		if (ally_frozen>enemy_frozen) return 0;
		if (get_hp(COMPUTER_PLAYER,card_pos)<=400&&deploy_status[COMPUTER_PLAYER]) return 0;
	}

	if (char_num==104||char_num==105||char_num==106||char_num==107||char_num==110)
	{
		if (!count_chars(HUMAN_PLAYER)) return 0;
		if (count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)) return 0;
	}

	if (char_num==114)
	{
		if (!count_chars(HUMAN_PLAYER)) return 0;
		if (count_chars(HUMAN_PLAYER)>count_chars(COMPUTER_PLAYER)) return 0;
		if (deploy_status[COMPUTER_PLAYER]) return 0;
	}
	
	return 1;
}

int check_abilities_req(int card_pos)
{
	if (combat_area_card_type[COMPUTER_PLAYER][card_pos] == SNK_CARD) return check_abilities_req_SNK(card_pos);
	if (combat_area_card_type[COMPUTER_PLAYER][card_pos] == CAPCOM_CARD) return check_abilities_req_CAPCOM(card_pos);
	return 0;
}

int select_best_ability(int ai_value, int num_matches)
{
	if (!num_matches) return 0;
	
	int chars[3];
	chars[0] = 0;	
	chars[1] = 0;	
	chars[2] = 0;	
	
	int num_meets_req = 0;
	
	for (int i=0; i<3; i++)
	{
		if (combat_area[COMPUTER_PLAYER][i]!=255&&check_char_ai_value(i,AI_ABILITY)==ai_value)
		{
			if (check_abilities_req(i))
			{
				num_meets_req++;

				if (!chars[0])
				{
					chars[0] = i+1;
				}
				else
				{
					if (!chars[1]) chars[1] = i+1; else chars[2] = i+1;
				}
				
			}
		}
	}
	
	if (num_matches == 1)
	{
		if (num_meets_req) return chars[0];
	}

	if (num_meets_req == 1) return chars[0];
	
	if (chars[2])
	{
		int min_hp = 0;
		int min_hp_card = 0;
		
		for (int i=0; i<3; i++)
		{
			int char_hp = get_hp(COMPUTER_PLAYER,chars[i]-1);
			if (char_hp< min_hp||!min_hp_card)
			{
				min_hp = char_hp;
				min_hp_card = chars[i];
			}
		}
		
		return min_hp_card;
	}
	else
	{
		if (get_hp(COMPUTER_PLAYER,chars[0]-1)<get_hp(COMPUTER_PLAYER,chars[1]-1)) return chars[0]; else return chars[1];
	}
	
	return 0;
}

void do_computer_use_card_abilities()
{
	int current_ai = 10;
	int found_card = 0;

	if (count_chars(COMPUTER_PLAYER)<count_chars(HUMAN_PLAYER)) return;
	if (square_abilities_disabled[COMPUTER_PLAYER]) return;

	int num_attackers = 0;
	int num_blockers = 0;
		
	for (int i=0; i<3; i++)
	{
		if (can_block(HUMAN_PLAYER,i)) num_blockers++;
		if (can_attack(COMPUTER_PLAYER,i)) num_attackers++;	
	}
	
	if (num_attackers<num_blockers) return;
			
	while(current_ai&&!found_card)
	{
		int found_ai_matches = 0;
		
		for (int i=0; i<3; i++)
		{
			if (check_char_ai_value(i,AI_ABILITY)==current_ai)
			{
				if (can_attack(COMPUTER_PLAYER,i))
				{
					found_ai_matches++;
				}
			}
		}
		
		if (found_ai_matches)
		{
			found_card = select_best_ability(current_ai,found_ai_matches);
		}
		
		current_ai--;
	}

	if (found_card)
	{
		int ability = NO_STATS;
		char *char_ability;
		
		if (combat_area_card_type[COMPUTER_PLAYER][found_card-1]==SNK_CARD)
		{
			char_ability = (char *)SNK_ABILITIES[combat_area[COMPUTER_PLAYER][found_card-1]+1];
		}
		else
		{
			char_ability = (char *)CAPCOM_ABILITIES[combat_area[COMPUTER_PLAYER][found_card-1]+1];
		}

		ability = ability_stats(char_ability);
		
		if (ability==STATS_SQUARE)
		{
			use_special_abilities(COMPUTER_PLAYER,found_card-1);
		}
	}
}

int do_computer_place_card()
{
// this performs the "USE card from HAND" for the computer player
// this is composed of two steps:
// first the computer tries to put a character onto the combat area
// --- if it does, it decides between maximizing the attack capabilities
// or maximizing SPs for special actions
// it determines whether or not it can use an action card to give it advantage
//
    computer_deployed_card = 0;
    int vacant_slots = 0;
    
    for (int i=0;i<3;i++)
    {
        if (combat_area[COMPUTER_PLAYER][i] == 255) vacant_slots++;
    }
    
    // check if there are vacant slots
    if (vacant_slots)
    {
        if (!deploy_status[COMPUTER_PLAYER])
        {
                int max_hp = 0;
                int max_sp = 0;
                int max_hp_card = 0;
                int max_sp_card = 0;
                
                for (int i=0; i<MAX_HAND_SIZE; i++)
                {
                    if ((hand_cards[COMPUTER_PLAYER][i]!=255)&&hand_card_type[COMPUTER_PLAYER][i]==SNK_CARD||hand_card_type[COMPUTER_PLAYER][i]==CAPCOM_CARD)
                    {
                         if (hand_card_type[COMPUTER_PLAYER][i] == SNK_CARD)
                         {
                              int char_hp = SNK_chars_hp[hand_cards[COMPUTER_PLAYER][i]];
                              if (char_hp >= max_hp)
                              {
                                   max_hp = char_hp;
                                   max_hp_card = i + 1;
                              }

                              int char_sp = SNK_chars_sp[hand_cards[COMPUTER_PLAYER][i]];
                              if (char_sp >= max_sp)
                              {
                                   max_sp = char_sp;
                                   max_sp_card = i + 1;
                              }
                         }
                         else
                         {
                              int char_hp = Capcom_chars_hp[hand_cards[COMPUTER_PLAYER][i]];
                              if (char_hp >= max_hp)
                              {
                                   max_hp = char_hp;
                                   max_hp_card = i + 1;
                              }

                              int char_sp = Capcom_chars_sp[hand_cards[COMPUTER_PLAYER][i]];
                              if (char_sp >= max_sp)
                              {
                                   max_sp = char_sp;
                                   max_sp_card = i + 1;
                              }
                         }
                    }
                }
                
                if (vacant_slots >= 2)
                {
                     int card_deployed = 0;
                     int deploy_pos = 0;
                     
                     for (int i=0; i<3; i++)
                     {
                          if (!card_deployed&&combat_area[COMPUTER_PLAYER][i] == 255)
                          {
                                card_deployed = 1;
                                deploy_pos = i;
                          }
                     }
                     
                     if (card_deployed)
                     {
                          place_card(deploy_pos,COMPUTER_PLAYER,max_hp_card-1);
                          computer_deployed_card = deploy_pos+1;
                          set_deploy_status(COMPUTER_PLAYER);
                     }
                     else
                     {
                     	computer_deployed_card = 0;
                     }
                }
                else
                {
                     if (player_sp[COMPUTER_PLAYER]<5&&player_hp[COMPUTER_PLAYER]>500)
                     {
                          int card_deployed = 0;
                          int deploy_pos = 0;
                     
                          for (int i=0; i<3; i++)
                          {
                               if (!card_deployed&&combat_area[COMPUTER_PLAYER][i] == 255)
                               {
                                    card_deployed = 1;
                                    deploy_pos = i;
                               }
                          }
                     
                          if (card_deployed)
                          {
                               place_card(deploy_pos,COMPUTER_PLAYER,max_sp_card -1);
                               computer_deployed_card = deploy_pos+1;
                               set_deploy_status(COMPUTER_PLAYER);
                          }
                          else
                          {
                          	computer_deployed_card = 0;
                          } 
                     }
                     else
                     {
                          int card_deployed = 0;
                          int deploy_pos = 0;
                     
                          for (int i=0; i<3; i++)
                          {
                               if (!card_deployed&&combat_area[COMPUTER_PLAYER][i] == 255)
                               {
                                    card_deployed = 1;
                                    deploy_pos = i;
                               }
                          }
                     
                          if (card_deployed)
                          {
                               place_card(deploy_pos,COMPUTER_PLAYER,max_hp_card -1);
                               computer_deployed_card = deploy_pos+1;
                               set_deploy_status(COMPUTER_PLAYER);
                          }
                          else
                          {
                          	computer_deployed_card = 0;
                          }
                     }
                }
        }
    }

    if (deploy_status[COMPUTER_PLAYER]) return 1; else return 0;
}

int do_computer_view_hand()
{
// this allows the computer to select which card to play
// it can deploy character cards 
// or it can play action cards

// first check if there are characters that can be given backups
    computer_deployed_card = 0;
    
    for (int i=0;i<MAX_HAND_SIZE;i++)
    {
        if (hand_cards[COMPUTER_PLAYER][i]!=255&&hand_cards[COMPUTER_PLAYER][i]!=ACTION_CARD)
        {
             int backups = check_for_backups(COMPUTER_PLAYER,i);
             
             if (backups)
             {
                  if (check_backup(COMPUTER_PLAYER,i,backups-1))
                  {
                  	int backup_hp = 0;
                  	
                  	if (hand_card_type[COMPUTER_PLAYER][i]==SNK_CARD)
                  	{
				backup_hp = SNK_chars_hp[hand_cards[COMPUTER_PLAYER][i]];	
                  	}
                  	else
                  	{
				backup_hp = Capcom_chars_hp[hand_cards[COMPUTER_PLAYER][i]];	
                  	}
                  	
                  	// Check if backuper's HP is very much greater than the backuped
                  	// character's. If so then do not perform backup.
                  	if (backup_hp<=(combat_area_card_hp[COMPUTER_PLAYER][backups-1]+500))
                  	{
                  		computer_backup = 1;
                  		computer_backup_card = backups;
                  		do_backup(COMPUTER_PLAYER,i,backups-1);
                	}
                  }
             }
        }
    }
    
    // check if there are any useful cards that can be used by the computer
    do_computer_use_special_card();

    //if (!deploy_status[COMPUTER_PLAYER]) do_computer_place_card();
    
    if (!deploy_status[COMPUTER_PLAYER]) do_computer_selects_char_to_deploy();
  
    return 0;
}

void do_computer_block(int attack_pos1, int attack_pos2, int attack_pos3, int united_attack1, int united_attack2)
{
    // this determines the blocking actions performed by the computer
    // it is not smart --- no strategy is formulated by the computer;
    // computer block action is determined by performing elementary comparisons
    // of character statistics
    
    int block_status[3];
    int strongest_blocker = 0;
    int strongest_blocker_hp = 0;
    int num_blockers = 0;
    int weakest_blocker_hp = 0;
    int weakest_blocker = 0;
    
    int num_attackers = 0;
    
    if (attack_pos1) num_attackers++;
    if (attack_pos2) num_attackers++;
    if (attack_pos3) num_attackers++;
    if (united_attack1) num_attackers++;
    
    int strongest_united_blocker = 0;
    
    // check if there are potential blockers
    for (int i=0;i<3;i++)
    {
        block_status[i] = 0;            // clear block status
        computer_block_pos[i] = 255;    // clear block status
        
        if (can_block(COMPUTER_PLAYER,i))
        {
                num_blockers++;
                
                // determine strongest blocker
                int blocker_hp = get_hp(COMPUTER_PLAYER,i);
                
                if (blocker_hp>strongest_blocker_hp)
                {
                     strongest_blocker_hp = blocker_hp;
                     strongest_blocker = i+1;
                }
                
                if (blocker_hp<weakest_blocker_hp||!weakest_blocker)
                {
                     weakest_blocker = i+1;
                     weakest_blocker_hp = blocker_hp;
                }
        }
    }
    
    if (!num_blockers||!num_attackers) return;     // no blockers available or any attackers

    if (num_attackers == 1)
    {
	if (!check_if_blockable(HUMAN_PLAYER,attack_pos1-1)) return;
    }
    
    // check for special blockers
    if (!circle_abilities_disabled[COMPUTER_PLAYER])
    {
    	for (int i=0; i<3; i++)
    	{
    		if (can_block(COMPUTER_PLAYER,i))
    		{
    			if (combat_area_card_type[COMPUTER_PLAYER][i]==CAPCOM_CARD)
    			{
    				if (combat_area[COMPUTER_PLAYER][i]==44)
    				{
					strongest_blocker = i+1;
					strongest_united_blocker = i+1;    					
    				}
    				
    				if (combat_area[COMPUTER_PLAYER][i]==42)
    				{
    					if (!strongest_united_blocker) strongest_united_blocker = i+1;
    				}
    			}
    			else
    			{
    				if (combat_area[COMPUTER_PLAYER][i]==99)
    				{
    					if (!strongest_united_blocker) strongest_united_blocker = i+1;
    				}
    			}
    		}
    	}
    }
    
    // check if human player performed a united attack
    if (united_attack1)
    {
        // human player performed a united attack
        // if it is a potential threat (can significantly damage computer player HP, find strongest blocker to block it
        // otherwise check if at least 1 can be destroyed by strongest blocker --- if it can be destroyed, block it
        // otherwise ignore it

	int other_hp = 0;
	
	if (attack_pos1) other_hp = get_hp(HUMAN_PLAYER,attack_pos1-1);
        int total_hp = get_hp(HUMAN_PLAYER,united_attack1-1)+get_hp(HUMAN_PLAYER,united_attack2-1);
        
        if (total_hp>=(player_hp[COMPUTER_PLAYER]/(3*ai_scale_factor)))
        {
        	// united attack is a threat
        	// but check if stray attacker is stronger
        	
        	// if stray attacker is stronger block that one
        	// otherwise proceed with the block
        	if (other_hp<total_hp)
        	{
        		if (strongest_united_blocker)
        		{
                		computer_block_pos[1] = strongest_united_blocker;
                		block_status[strongest_united_blocker - 1] = 1;  
                		num_blockers --;       			
        		}
        		else
        		{
                		computer_block_pos[1] = strongest_blocker;
                		block_status[strongest_blocker - 1] = 1;  
                		num_blockers --;
                	}
                }
                else
                {
                	if (num_blockers == 2)
                	{
                		computer_block_pos[1] = weakest_blocker;
                		block_status[weakest_blocker - 1] = 1;  
                		num_blockers --;
                	}

                	if (num_blockers == 3)
                	{
                		for (int i=0; i<3; i++)
                		{
                			if (i+1!=weakest_blocker&&i+1!=strongest_blocker)
                			{
                				computer_block_pos[1] = i+1;
                				block_status[i] = 1;  
                				num_blockers --;
                			}
                		}
                	}
                }
        }
        else
        {
                // Not a potential threat to computer HP
                // check if weakest blocker can destroy at least one of it
                // if it can -- then block it otherwise ignore it completely
                if (weakest_blocker_hp>=total_hp||weakest_blocker_hp>=get_hp(HUMAN_PLAYER,united_attack2-1))
                {
                	if (weakest_blocker!=strongest_blocker) // check if weakest blocker is the same as strongest blocker
                     	{
                     		computer_block_pos[1] = weakest_blocker;
                     		block_status[weakest_blocker - 1] = 1;
                     		num_blockers--;
                     	}
                }
        }
        
        // check if there are other attackers
        if (attack_pos1)
        {
                if (!num_blockers||!check_if_blockable(HUMAN_PLAYER,attack_pos1-1)) return;
                
                // check if single attacker is a threat
                if (get_hp(HUMAN_PLAYER,attack_pos1-1)>=player_hp[COMPUTER_PLAYER]/(3*ai_scale_factor))
                {
                     // check if strongest blocker blocked the united attack
                     // if it didn't block the united attack, block the stray
                     // attacker otherwise look for the next
                     if (!block_status[strongest_blocker-1])
                     {
                          computer_block_pos[0] = strongest_blocker;
                     }
                     else
                     {
                          int next_strongest_blocker = 0;
                          int next_strongest_blocker_hp = 0;
                          
                          for (int i = 0; i<3; i++)
                          {
                               if (can_block(COMPUTER_PLAYER,i)&&(i+1)!=strongest_blocker)
                               {
                                    int next_blocker_hp = get_hp(COMPUTER_PLAYER,i);
                                    if (next_blocker_hp > next_strongest_blocker_hp)
                                    {
                                         next_strongest_blocker = i + 1;
                                         next_strongest_blocker_hp = next_blocker_hp;
                                    }
                               }
                          }
                          
                          if (next_strongest_blocker) computer_block_pos[0] = next_strongest_blocker;
                     }
                }
                else
                {
                     // stray attacker is not a threat to computer HP
                     // check if it can be destroyed. if it can be destroyed
                     // block it
                     
                     // check first if weakest blocker can still block it
                     if (!block_status[weakest_blocker-1])
                     {
                          if (weakest_blocker_hp>=get_hp(HUMAN_PLAYER,attack_pos1-1)) computer_block_pos[0] = weakest_blocker;
                     }
                     else
                     {
                           // look for another blocker
                           int next_blocker = 0;
                           int next_blocker_hp = 0;
                           
                           for (int i=0; i<3; i++)
                           {
                                if (can_block(COMPUTER_PLAYER,i)&&(i+1)!=weakest_blocker&&(i+1)!=strongest_blocker)
                                {
                                     int blocker_hp = get_hp(COMPUTER_PLAYER,i);
                                     
                                     if (blocker_hp >= get_hp(COMPUTER_PLAYER,attack_pos1-1))
                                     {
                                          if (blocker_hp<=next_blocker_hp||!next_blocker)
                                          {
                                               next_blocker = i+1;
                                               next_blocker_hp = blocker_hp;
                                          }
                                     }
                                }
                           }
                           
                           if (next_blocker) computer_block_pos[0] = next_blocker;
                     }
                }
        }
    }
    else
    {
        // human player did not perform a united attack
        int attackers[3];
        int attack_order[3];
        int num_attackers = 0;
        int min_attacker_hp = 0;
        int min_attacker = 0;
        int max_attacker_hp = 0;
        int max_attacker = 0;
    	int computer_hp = player_hp[COMPUTER_PLAYER];                   
        attackers[0] = attack_pos1;
        attackers[1] = attack_pos2;
        attackers[2] = attack_pos3;
        
        for (int j=0; j<3; j++)
        {
             if (attackers[j])
             {
                  num_attackers++;
                  if (get_hp(HUMAN_PLAYER,attackers[j]-1)>max_attacker_hp)
                  {
                       max_attacker_hp = get_hp(HUMAN_PLAYER,attackers[j]-1);
                       max_attacker = j+1;
                  }
                  
                  if (get_hp(HUMAN_PLAYER,attackers[j]-1)<min_attacker_hp || !min_attacker)
                  {
                       min_attacker_hp = get_hp(HUMAN_PLAYER,attackers[j]-1);
                       min_attacker = j+1;
                  }
             }
             
             attack_order[j] = j+1;
        }

        // this sorts the attackers in decreasing hp
        // (see code before end of this code subroutine)
        if (num_attackers == 2)
        {
             	if (min_attacker<max_attacker)
             	{
			attack_order[min_attacker - 1] = max_attacker;
                  	attack_order[max_attacker - 1] = min_attacker;
                  	int temp = attackers[max_attacker-1];
                  	attackers[max_attacker-1] = attackers[min_attacker-1];
                  	attackers[min_attacker-1] = temp;
             	}
        }
        
        if (num_attackers == 3)
        {
             if (min_attacker != 3)
             {
                  attack_order[min_attacker-1] = attack_order[2];
                  attack_order[2] = min_attacker;
                  int temp = attackers[min_attacker-1];
                  attackers[min_attacker-1] = attackers[2];
                  attackers[2] = temp;
             }
             
             if (get_hp(HUMAN_PLAYER,attackers[0]-1)<get_hp(HUMAN_PLAYER,attackers[1]-1))
             {
                  int temp = attack_order[0];
                  attack_order[0] = attack_order[1];
                  attack_order[1] = temp;
                  temp = attackers[0];
                  attackers[0] = attackers[1];
                  attackers[1] = temp;
             }
        }
        
        for (int j = 0; j<3; j++)
        {
          if (num_blockers)
          {
     		// if attacker is present
                // try to match a blocker
                if (attackers[j]&&check_if_blockable(HUMAN_PLAYER,attackers[j]-1))
                {
                     // check if attacker is threatening to the computer hp
                     if (get_hp(HUMAN_PLAYER,attackers[j]-1)>=(computer_hp/(3*ai_scale_factor)))
                     {
                          //it is a threat --- find a blocker to match it
                          // check if strongest blocker can still block it
                          if (!block_status[strongest_blocker-1])
                          {
                               block_status[strongest_blocker-1] = 1;
                               computer_block_pos[j] = strongest_blocker;
                               num_blockers--;
                          }
                          else
                          {
                               // try to find next strongest blocker
                               int blocker = 0;
                               int blocker_hp = 0;
                                    
                               for (int i = 0; i<3; i++)
                               {
                                    if (can_block(COMPUTER_PLAYER,i)&&!block_status[i])
                                    {
                                         int next_blocker_hp = get_hp(COMPUTER_PLAYER,i);
                                         if (next_blocker_hp >= blocker_hp)
                                         {
                                              blocker = i+1;
                                              blocker_hp = next_blocker_hp;
                                         }
                                    }
                               }
                                    
                               if (blocker)
                               {
                                    block_status[blocker-1] = 1;
                                    computer_block_pos[j] = blocker;
                                    num_blockers --;
                               }
                               else
                               {
                        	    computer_hp -= get_hp(HUMAN_PLAYER,attackers[j]-1);  // modify hypothetical HP
			       }
                          }
                     }
                     else
                     {
                          // attacker is not a threat to computer hp
                          // check if it can be destroyed.
                          // if it can be destroyed find the minimum
                          // hp needed to destroy it
			int blocker_hp = 0;
                        int blocker = 0;
                          
			if (num_blockers>num_attackers)
                        {
                          	for (int i = 0; i<3; i++)
                          	{
                               		if (can_block(COMPUTER_PLAYER,i)&&!block_status[i])
                               		{
                                    		int next_blocker_hp = get_hp(COMPUTER_PLAYER,i);
                                    		if (next_blocker_hp >= get_hp(HUMAN_PLAYER,attackers[j]-1))
                                    		{
                                         		if (next_blocker_hp<=blocker_hp||!blocker)
                                         		{
                                              			blocker = i+1;
                                              			blocker_hp = next_blocker_hp;
                                         		}
                                    		}
                               		}
                               	}
                          
                          	if (blocker)
                          	{
                               		computer_block_pos[j] = blocker;
                               		block_status[blocker - 1] = 1;
                               		num_blockers --;
                          	}
                          	else
                          	{
                          		computer_hp -= get_hp(HUMAN_PLAYER,attackers[j]-1);
                          	}
			}
                     }
                }
          }
        }

        // sort-out the blocking positions in the original order
        for (int i=0; i<3; i++)
        {
		if (attack_order[i]!= i+1)
             	{
			int temp = computer_block_pos[attack_order[i]-1];
                  	computer_block_pos[attack_order[i]-1] = computer_block_pos[i];
                  	computer_block_pos[i] = temp;
                  	temp = attack_order[attack_order[i]-1];
                  	attack_order[attack_order[i]-1] = attack_order[i];
                  	attack_order[i] = temp;
             	}
        }
    }
}
