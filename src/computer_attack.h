const char computer_attacks [] = {"ATTACKS$"};
const char no_blockers [] = {"YOU DO NOT/HAVE ANY/BLOCKERS$"};
const char choose_blocker [] = {"CHOOSE BLOCKER$"};
const char choose_card_to_block [] = {"CHOOSE WHICH/CARD TO BLOCK$"};

const char block_error_msg_01 [] = {"YOU DO NOT OWN/THAT CHARACTER$"};
const char block_error_msg_02 [] = {"CANNOT/COUNTERATTACK$"};
const char block_error_msg_03 [] = {"CANNOT BLOCK/OWN CHARACTER$"};
const char block_error_msg_04 [] = {"ALREADY/BLOCKED$"};
const char block_error_msg_05 [] = {"UNBLOCKABLE$"};
const char* block_error_msgs [] =  {block_error_msg_01,block_error_msg_02,block_error_msg_03,block_error_msg_04,block_error_msg_05};

void make_computer_attack(int attack_pos1, int attack_pos2, int attack_pos3, int united_pos1, int united_pos2)
{
// this informs the player of the computer's attack and allows the human to
// choose blockers
	int num_blockers = 0;
    	int block_status[3];

    	int attack_pos[3];
    	attack_pos[0] = attack_pos1;
    	attack_pos[1] = attack_pos2;
    	attack_pos[2] = attack_pos3;
	int num_attackers = 0;
    
    	for (int i=0; i<3; i++)
    	{
        	human_block_pos[i] = 255;
        	block_status[i] = 0;
        	if (can_block(HUMAN_PLAYER,i)) num_blockers++;
        	if (attack_pos[i]) num_attackers++;
    	}
    	
    	if (united_pos1) num_attackers++;

    	if (!num_attackers) return;

	int exit_flag = 0;

	if (num_attackers == 1)
	{
		if (!check_if_blockable(COMPUTER_PLAYER,attack_pos1-1))
		{
			exit_flag = 1;
			num_blockers = 0;
		}
	}
	
	InitializeSprites();
	putBG(back_offs[0]);
		
	putsprite(5,4,96,100,left_paneData);
	putsprite(54,4,120,100,right_paneData);

	view_combat_area (COMPUTER_PLAYER,STATUS_ATTACKING,attack_pos[0],attack_pos[1],attack_pos[2],united_pos1,united_pos2);
	view_combat_area (HUMAN_PLAYER,STATUS_BLOCKING,block_status[0],block_status[1],block_status[2],0,0);
	view_stats_area();
	
	PutString(16,14,(char *)player_names[COMPUTER_PLAYER],0x62); 
	PutString(16,22,(char *)computer_attacks,0x9D); 
	if (!num_blockers) PutString(16,38,(char *)no_blockers,0x9D); 

	WaitForVsync();
	Flip();
	CopyOAM();
	
	int pressed_key_a = 0;

	while(!pressed_key_a)
	{
		if (!(*KEYS&KEY_A)) pressed_key_a = 1;
	}

	int error_flag = 0;
	int player = HUMAN_PLAYER;
	int current_pos = 0;
	int selected_blocker = 0;
	int choose_flag = 0;

	if (!num_blockers) exit_flag = 1;
		
	while (!exit_flag)
	{
		InitializeSprites();
		putBG(back_offs[0]);
		
		putsprite(5,4,96,100,left_paneData);
		putsprite(54,4,120,100,right_paneData);

		view_combat_area (COMPUTER_PLAYER,STATUS_ATTACKING,attack_pos[0],attack_pos[1],attack_pos[2],united_pos1,united_pos2);
		view_combat_area (HUMAN_PLAYER,STATUS_BLOCKING,block_status[0],block_status[1],block_status[2],0,0);
		view_stats_area();

		print_area_chars(player,current_pos,PRINT_CARD_IN_PLAY);
		
		if (player == COMPUTER_PLAYER)
		{
			putspriteBG(56+current_pos*20+6,1+(1-player)*50,18,28,0,selectData);
		}
		else
		{
			putspriteBG(56+current_pos*20+6,1+(1-player)*50+40,18,28,0,pointerData);
		}

		if (!choose_flag&&!error_flag)
		{
			PutString(8,133,(char *)choose_blocker,0x9D); 
		}
		
		if (choose_flag&&!error_flag)
		{
			PutString(8,133,(char *)choose_card_to_block,0x9D); 
		}

		if (error_flag)
		{
			PutString(8,133,(char *)block_error_msgs[error_flag-1],0x9D); 
			error_flag = 0;
		}
		
		WaitForVsync();
		Flip();
		CopyOAM();

		int key_pressed = 0;
		
		if (united_pos1)
		{
			// computer performs a united attack
			while (!key_pressed)
			{
				if (!(*KEYS&KEY_UP))
				{
					while (!(*KEYS&KEY_UP));
					if (!player) player = 1 - player;
					key_pressed = 1;
				}			

				if (!(*KEYS&KEY_DOWN))
				{
					while (!(*KEYS&KEY_DOWN));
					if (player) player = 1 - player;
					key_pressed = 1;
				}
				
				if (!(*KEYS&KEY_LEFT))
				{
					while (!(*KEYS&KEY_LEFT));
					if (current_pos) current_pos--;
					key_pressed = 1;
				}

				if (!(*KEYS&KEY_RIGHT))
				{
					while (!(*KEYS&KEY_RIGHT));
					if (current_pos!=2) current_pos++;
					key_pressed = 1;
				}
				
				if (!(*KEYS&KEY_START))
				{
					while (!(*KEYS&KEY_START));
					
					for (int i=0;i<3;i++)
					{
						key_pressed = 1;

						if (block_status[i])
						{
							if (i+1==united_pos2&&block_status[united_pos1-1])
							{
								error_flag = 4;
							}
							else
							{
								if (i+1 == united_pos2)
								{
									swap_cards(HUMAN_PLAYER,i+1,united_pos1);
									block_status[i] = 0;
									block_status[united_pos1-1] = united_pos1;
								}
								else
								{
									if (i+1!=attack_pos[0]&&i+1!=united_pos1)
									{
										error_flag = 2;
									}
								}
							}
						}
					}
					
					if (!error_flag)
					{
						exit_flag = 2;
					}
				}

				if (!(*KEYS&KEY_A))
				{
					while (!(*KEYS&KEY_A));
					key_pressed = 1;
					
					if (player == HUMAN_PLAYER)
					{
						if (selected_blocker)
						{
							if (selected_blocker == current_pos+1)
							{
								block_status[current_pos] = 0;
								selected_blocker = 0;
								choose_flag = 0;
							}
							else
							{
								error_flag = 3;
							}
						}
						else
						{
							if (can_block(HUMAN_PLAYER,current_pos))
							{
								if (!block_status[current_pos])
								{
									block_status[current_pos] = current_pos+1;
									selected_blocker = current_pos+1;
									choose_flag = 1;
								}
								else
								{
									block_status[current_pos] = 0;
									choose_flag = 0;
								}
							}
							else
							{
								error_flag = 2;
							}
						}
					}
					else
					{
						if (selected_blocker)
						{
							if (block_status[current_pos])
							{
								int valid_block_pos = 0;
									
								if (current_pos+1==attack_pos[0]) valid_block_pos++;
								if (current_pos+1==united_pos1) valid_block_pos++;
								if (current_pos+1==united_pos2) valid_block_pos++;
									
								if (valid_block_pos)
								{
									if (!check_if_blockable(COMPUTER_PLAYER,current_pos))
									{
										error_flag = 5;
									}
									else
									{
										if (current_pos+1!=selected_blocker||current_pos+1==united_pos2&&block_status[united_pos1-1])
										{
											error_flag = 4;
										}
										else
										{
											block_status[selected_blocker-1] = 0;
										
											if (current_pos+1 == attack_pos[0]||current_pos+1==united_pos1)
											{
												swap_cards(HUMAN_PLAYER,selected_blocker,current_pos+1);
												block_status[current_pos] = current_pos+1;
											}
										
											if (current_pos+1 == united_pos2)
											{
												swap_cards(HUMAN_PLAYER,selected_blocker,united_pos1);
												block_status[current_pos] = 0;
												block_status[united_pos1-1] = united_pos1;
											}
										
											choose_flag = 0;
											selected_blocker = 0;
										}
									}
								}
								else
								{
									error_flag = 2;
								}
							}
							else
							{
								int valid_block_pos =0;
								
								if (current_pos+1==attack_pos[0]) valid_block_pos++;
								if (current_pos+1==united_pos1) valid_block_pos++;
								if (current_pos+1==united_pos2) valid_block_pos++;
								
								if (valid_block_pos)
								{
									if (!check_if_blockable(COMPUTER_PLAYER,current_pos))
									{
										error_flag = 5;
									}
									else
									{
										block_status[selected_blocker-1] = 0;
									
										if (current_pos+1 == attack_pos[0]||current_pos+1==united_pos1)
										{
											swap_cards(HUMAN_PLAYER,selected_blocker,current_pos+1);
											block_status[current_pos] = current_pos+1;
										}
										
										if (current_pos+1 == united_pos2)
										{
											swap_cards(HUMAN_PLAYER,selected_blocker,united_pos1);
											block_status[current_pos] = 0;
											block_status[united_pos1-1] = united_pos1;
										}
										
										choose_flag = 0;
										selected_blocker = 0;
									}
								}
								else
								{
									error_flag = 2;
								}
							}
						}
						else
						{
							error_flag = 1;
						}
					}
				}
			}
		}
		else
		{
			// computer did not perform a united attack
			while (!key_pressed)
			{
				if (!(*KEYS&KEY_UP))
				{
					while (!(*KEYS&KEY_UP));
					if (!player) player = 1 - player;
					key_pressed = 1;
				}			

				if (!(*KEYS&KEY_DOWN))
				{
					while (!(*KEYS&KEY_DOWN));
					if (player) player = 1 - player;
					key_pressed = 1;
				}
				
				if (!(*KEYS&KEY_LEFT))
				{
					while (!(*KEYS&KEY_LEFT));
					if (current_pos) current_pos--;
					key_pressed = 1;
				}

				if (!(*KEYS&KEY_RIGHT))
				{
					while (!(*KEYS&KEY_RIGHT));
					if (current_pos!=2) current_pos++;
					key_pressed = 1;
				}

				if (!(*KEYS&KEY_START))
				{
					while (!(*KEYS&KEY_START));
					key_pressed = 1;
					
					for (int i=0;i<3;i++)
					{
						if (block_status[i])
						{
							if (i+1!=attack_pos[0]&&i+1!=attack_pos[1]&&i+1!=attack_pos[2])
							{
								error_flag = 2;
							}
						}
					}
					
					if (!error_flag)
					{
						exit_flag = 1;
					}
				}
				
				if (!(*KEYS&KEY_A))
				{
					while (!(*KEYS&KEY_A));
					key_pressed = 1;
					
					if (player == HUMAN_PLAYER)
					{
						if (selected_blocker)
						{
							if (selected_blocker == current_pos+1)
							{
								block_status[current_pos] = 0;
								selected_blocker = 0;
							}
							else
							{
								error_flag = 3;
							}
						}
						else
						{
							if (can_block(HUMAN_PLAYER,current_pos))
							{
								if (!block_status[current_pos])
								{
									block_status[current_pos] = current_pos+1;
									selected_blocker = current_pos+1;
									choose_flag = 1;
								}
								else
								{
									block_status[current_pos] = 0;
								}
							}
							else
							{
								error_flag = 2;
							}
						}
					}
					else
					{
						if (selected_blocker)
						{
							if (block_status[current_pos])
							{
								if (selected_blocker!=current_pos+1)
								{
									error_flag = 4;
								}
								else
								{
									int valid_block_pos = 0;
									
									for (int i=0;i<3;i++)
									{
										if (current_pos+1==attack_pos[i]) valid_block_pos++;
									}
									
									if (valid_block_pos)
									{
										swap_cards(HUMAN_PLAYER,selected_blocker,current_pos+1);
										block_status[selected_blocker-1] = 0;
										block_status[current_pos] = current_pos+1;
										selected_blocker = 0;
										choose_flag = 0;
									}
									else
									{
										error_flag = 2;
									}
								}
							}
							else
							{
								int valid_block_pos =0;
								
								for (int i=0;i<3;i++)
								{
									if (current_pos+1==attack_pos[i]) valid_block_pos++;
								}
								
								if (valid_block_pos)
								{
									swap_cards(HUMAN_PLAYER,selected_blocker,current_pos+1);
									block_status[selected_blocker-1] = 0;
									block_status[current_pos] = current_pos+1;
									selected_blocker = 0;
									choose_flag = 0;
								}
								else
								{
									error_flag = 2;
								}
							}
						}
						else
						{
							error_flag = 1;
						}
					}
				}
			}
		}
	}
	
	if (united_pos1)
	{
		for (int i=0;i<3;i++)
		{
			if (united_pos1 == block_status[i]) human_block_pos[1] = block_status[i];
			if (attack_pos[0] == block_status[i]) human_block_pos[0] = block_status[i];
		}
	}
	else
	{
		for (int j=0;j<3;j++)
		{
			if (attack_pos[j])
			{
				for (int i=0;i<3;i++)
				{
					if (attack_pos[j] == block_status[i])
					{
						human_block_pos[j] = attack_pos[j];
					}
				}
			}
		}
	}

    	InitializeSprites();
	putBG(back_offs[0]);
	WaitForVsync();
	Flip();
	CopyOAM();
}

void do_computer_attack()
{
// this determines the attacking actions performed by the computer
// it is not smart --- no strategy is formulated by the computer.
// computer attack action is determined by performing comparisons
    int num_chars = 0;
    int num_attackers = 0;
    
    int num_blockers = 0;
    int num_nonblockers = 0;

    int max_nonblocker_hp = 0;
    int min_nonblocker_hp = 0;
    int min_nonblocker = 0;
    int max_blocker_hp = 0;
    
    int max_attacker_hp = 0;
    int min_attacker_hp = 0;
    
    int strongest_attacker = 0;

    int attack_status[3];
    int make_united_attack = 0;
    
    int computer_attack_pos[3];
    int computer_attack_pos1 = 0;
    int computer_attack_pos2 = 0;

    for (int i=0; i<3; i++)
    {
        attack_status[i] = 0;
        computer_attack_pos[i] = 0;
        
        int computer_hp = get_hp(COMPUTER_PLAYER,i);
        int human_hp = get_hp(HUMAN_PLAYER,i);
        
        // determine number of attackers 
        if (can_attack(COMPUTER_PLAYER,i)) 
        {
                num_attackers++;
                if (computer_hp>=max_attacker_hp)
                {
                     max_attacker_hp = computer_hp;
                     strongest_attacker = i+1;
                }
                
                // determine the weakest attacker
                if (computer_hp<=min_attacker_hp||!min_attacker_hp)
                {
                     min_attacker_hp = computer_hp;
                }
        }
        
        // determine characters in human player's combat area
        if (combat_area[HUMAN_PLAYER][i]!=255)
        {
                num_chars++;
                if (!can_block(HUMAN_PLAYER,i))
                {
                     num_nonblockers++;
                     // determine strongest non-blocking character human player's combat area
                     if (human_hp>=max_nonblocker_hp) max_nonblocker_hp = human_hp;
                     if (human_hp<min_nonblocker_hp||!min_nonblocker)
                     {
                     	min_nonblocker_hp = human_hp;
                     	min_nonblocker = i+1;
                     }
                }
        }
        
        // determine if human player has any blockers
        if (can_block(HUMAN_PLAYER,i))
        {
                num_blockers++;
                if (human_hp>=max_blocker_hp) max_blocker_hp = human_hp;
        }
    }

    // check if an attack is possible
    if (!num_attackers) return;    

    // check if it is a suicidal attack
    if (num_attackers == 1&&num_blockers == 3) return;
    if (num_attackers == 1&&(num_nonblockers==num_chars&&num_chars!=0&&player_hp[HUMAN_PLAYER]>get_hp(COMPUTER_PLAYER,strongest_attacker-1))) return;
    
    // if there are no blockers in human player's area then proceed to full attack!
    if (!num_chars)
    {
        // attack with all available attackers
        for (int i=0; i<3; i++)
        {
                if (can_attack(COMPUTER_PLAYER,i)) computer_attack_pos[i] = i + 1;
        }
    }
    else
    {
        // there are characters in the area ---
        // proceed cautiously
        
        // check if there are no immediate blockers
        if (num_nonblockers == num_chars)
        {
             // check if computer can significantly damage human player and survive a counter attack
             int total_damage = 0;
             int total_counter_damage = 0;
             
             for (int i = 0; i<3; i++)
             {
                  if (combat_area[HUMAN_PLAYER][i]!=255) total_counter_damage += get_hp(HUMAN_PLAYER,i);
                  if (can_attack(COMPUTER_PLAYER,i)) total_damage += get_hp(COMPUTER_PLAYER,i);
             }
             
             // if it can significantly damage human player and survive then proceed with full attack
             if ((total_damage>player_hp[HUMAN_PLAYER]/(2*ai_scale_factor))&&total_counter_damage<player_hp[COMPUTER_PLAYER]/(2*ai_scale_factor)||total_damage>=player_hp[HUMAN_PLAYER])
             {
                     for (int i=0; i<3; i++)
                     {
                          if (can_attack(COMPUTER_PLAYER,i)) computer_attack_pos[i] = i+1;
                     }
             }
        }
        else
        {
             // there are immediate blockers

             // check if all blockers can be destroyed by the attack
             // regardless of the blocking configurations
             if (num_attackers >= num_blockers)
             {
                  if (num_attackers == num_blockers)
                  {
                       if (max_blocker_hp>min_attacker_hp)
                       {
                            // not everyone will be destroyed
                            // check if a united attack can be made
                            // so that it can destroy strongest blocker
                            if (player_sp[COMPUTER_PLAYER]>5&&num_attackers==2)
                            {
                                 for (int i=0; i<3; i++)
                                 {
                                      if (can_attack(COMPUTER_PLAYER,i))
                                      {
                                           if (!computer_attack_pos1)
                                           {
                                                computer_attack_pos1 = i+1;
                                           }
                                           else
                                           {
                                                if (!computer_attack_pos2) computer_attack_pos2 = i+1;
                                           }
                                      }
                                 }
                                      
                                 // if it cannot be destroyed do not proceed with attack
                                 if ((get_hp(COMPUTER_PLAYER,computer_attack_pos1-1)+get_hp(COMPUTER_PLAYER,computer_attack_pos2-1)<max_blocker_hp))
                                 {
                                      computer_attack_pos1 = 0;
                                      computer_attack_pos2 = 0;
                                 }
                                 else
                                 {
                                      make_united_attack = 1;
                                      if (get_hp(COMPUTER_PLAYER,computer_attack_pos1-1)>get_hp(COMPUTER_PLAYER,computer_attack_pos2-1))
                                      {
                                           int temp = computer_attack_pos1;
                                           computer_attack_pos1 = computer_attack_pos2;
                                           computer_attack_pos2 = temp;
                                      }
                                 }
                            }
                            else
                            {
                                 // cannot perform a united attack
                                 // check if strongest attacker
                                 // can destroy strongest blocker
                                 if (max_attacker_hp>=max_blocker_hp) computer_attack_pos[0] = strongest_attacker;
                            }
                       }
                       else
                       {
                            // proceed with attack
                            for (int i = 0; i<3; i++)
                            {
                                 if (can_attack(COMPUTER_PLAYER,i)) computer_attack_pos[i] = i+1;
                            }
                       }
                  }
                  else
                  {
                       // computer has more attackers
                       // check if you can make a united attack
                       if (player_sp[COMPUTER_PLAYER]>5)
                       {
                            if (num_attackers == 2)
                            {
                                 for (int i=0; i<3; i++)
                                 {
                                      if (can_attack(COMPUTER_PLAYER,i))
                                      {
                                           if (!computer_attack_pos1)
                                           {
                                                computer_attack_pos1 = i+1;
                                           }
                                           else
                                           {
                                                if (!computer_attack_pos2) computer_attack_pos2 = i+1;
                                           }
                                      }
                                 }
                                      
                                 // if it cannot be destroyed do not proceed with attack
                                 if ((get_hp(COMPUTER_PLAYER,computer_attack_pos1-1)+get_hp(COMPUTER_PLAYER,computer_attack_pos2-1)<max_blocker_hp))
                                 {
                                      computer_attack_pos1 = 0;
                                      computer_attack_pos2 = 0;
                                 }
                                 else
                                 {
                                      make_united_attack = 1;
                                      if (get_hp(COMPUTER_PLAYER,computer_attack_pos1-1)>get_hp(COMPUTER_PLAYER,computer_attack_pos2-1))
                                      {
                                           int temp = computer_attack_pos1;
                                           computer_attack_pos1 = computer_attack_pos2;
                                           computer_attack_pos2 = temp;
                                      }
                                 }
                            }
                            else
                            {
                                 // computer has three attackers
                                 // determine minimum configuration requred to destroy
                                 // strongest blocker
                                 int config[3];
                                 
                                 int min_config_hp = 0;
                                 int min_config = 0;
                                 
                                 config[0] = get_hp(COMPUTER_PLAYER,0)+get_hp(COMPUTER_PLAYER,1);
                                 config[1] = get_hp(COMPUTER_PLAYER,0)+get_hp(COMPUTER_PLAYER,2);
                                 config[2] = get_hp(COMPUTER_PLAYER,1)+get_hp(COMPUTER_PLAYER,2);
                                 
                                 for (int i = 0; i<3; i++)
                                 {
                                      if (config[i]<= min_config_hp||!min_config_hp)
                                      {
                                           min_config_hp = config[i];
                                           min_config = i;
                                      }
                                 }
                                 
                                 if (min_config_hp>=max_blocker_hp&&(max_nonblocker_hp+min_nonblocker_hp)<player_hp[COMPUTER_PLAYER]/(2*ai_scale_factor))
                                 {
                                      if (min_config == 0) { computer_attack_pos1 = 1; computer_attack_pos2 = 2; }
                                      if (min_config == 1) { computer_attack_pos1 = 1; computer_attack_pos2 = 3; }
                                      if (min_config == 2) { computer_attack_pos1 = 2; computer_attack_pos2 = 3; }
                                      
                                      make_united_attack = 1;
                                      
                                      if (get_hp(COMPUTER_PLAYER,computer_attack_pos1-1)>get_hp(COMPUTER_PLAYER,computer_attack_pos2-1))
                                      {
                                           int temp = computer_attack_pos1;
                                           computer_attack_pos1 = computer_attack_pos2;
                                           computer_attack_pos2 = temp;
                                      }
                                 }
                                 
                                 // if a united attack is feasible check if the other can
                                 // also attack 
                                 if (make_united_attack)
                                 {
                                      int other_attacker = 0;
                                 
                                      for (int i = 0; i<3; i++)
                                      {
                                           if ((computer_attack_pos1!=(i+1))&&(computer_attack_pos2!=(i+1))) other_attacker = i+1;
                                      }
                                 
                                      if (get_hp(COMPUTER_PLAYER,other_attacker-1)>=max_blocker_hp) computer_attack_pos[0] = other_attacker; 
                                 }
                                 else
                                 {
                                      // united attack is not feasible
                                      // determine whether an all out attack can
                                      // eliminate the strongest defender
                                      if (min_attacker_hp>=max_blocker_hp&&(max_nonblocker_hp+min_nonblocker_hp)<player_hp[COMPUTER_PLAYER]/(2*ai_scale_factor))
                                      {
                                           for (int i = 0; i<3; i++)
                                           {
                                                computer_attack_pos[i] = i+1;
                                           }
                                      }
                                      else
                                      {
                                           // all=out attack is not feasible
                                           // check if strongest attacker
                                           // can take out strongest blocker
                                           if (max_attacker_hp>=max_blocker_hp)
                                           {
                                                computer_attack_pos[0] = strongest_attacker; 
                                           }
                                      }
                                 }
                            }
                       }
                       else
                       {
                            // cannot perform a united attack
                            // check if all-out attack can
                            // take out every blocker
                            if (min_attacker_hp>=max_blocker_hp&&(max_nonblocker_hp+min_attacker_hp)<player_hp[COMPUTER_PLAYER]/(2*ai_scale_factor))
                            {
                                 for (int i = 0; i<3; i++)
                                 {
                                      computer_attack_pos[i] = i+1;
                                 }
                            }
                            else
                            {
                                 // all-out attack is not feasible
                                 // check if strongest attacker
                                 // can take out strongest blocker
                                 if (max_attacker_hp>=max_blocker_hp)
                                 {
                                      computer_attack_pos[0] = strongest_attacker; 
                                 }
                            }
                       }
                  }
             }
             else
             {
                  // computer has fewer attackers
                  // check if an all-out attack is feasible
                  if (min_attacker_hp>=max_blocker_hp&&(max_nonblocker_hp+min_nonblocker_hp)<player_hp[COMPUTER_PLAYER]/(2*ai_scale_factor))
                  {
                       for (int i = 0; i<3; i++)
                       {
                            if (can_attack(COMPUTER_PLAYER,i)) computer_attack_pos[i] = i+1;
                       }
                  }
                  else
                  {
                       // all=out attack is not feasible
                       // check if strongest attacker
                       // can take out strongest blocker
                       if (max_attacker_hp>=max_blocker_hp)
                       {
                            computer_attack_pos[0] = strongest_attacker; 
                       }
                  }
             }
        }
    }
    
    // perform actual attack
    if (make_united_attack)
    {
        if (computer_attack_pos[0])
        {
             make_computer_attack(computer_attack_pos[0],0,0,computer_attack_pos1,computer_attack_pos2);
             united_attack(COMPUTER_PLAYER,computer_attack_pos1,computer_attack_pos2,human_block_pos[1]);
             if (player_hp[HUMAN_PLAYER]) card_battle(COMPUTER_PLAYER,computer_attack_pos[0],human_block_pos[0]);
        }
        else
        {
             make_computer_attack(0,0,0,computer_attack_pos1,computer_attack_pos2);
             united_attack(COMPUTER_PLAYER,computer_attack_pos1,computer_attack_pos2,human_block_pos[1]);
        }
    }
    else
    {
        make_computer_attack(computer_attack_pos[0],computer_attack_pos[1],computer_attack_pos[2],0,0);
        for (int i=0;i<3;i++)
        {
                if (computer_attack_pos[i]&&player_hp[HUMAN_PLAYER]) card_battle(COMPUTER_PLAYER,computer_attack_pos[i],human_block_pos[i]);
        }
    }
    
    InitializeSprites();
    putBG(back_offs[0]);
    WaitForVsync();
    Flip();
    CopyOAM();
}

void do_computer_replace_card()
{
// this allows the computer to make a bluff attack
// in order to replace a card in his combat area

    // check if computer has already deployed a character
    if (deploy_status[COMPUTER_PLAYER]) return;
    
    int num_chars = 0;
    int weakest_char = 0;
    int weakest_hp = 0;
    
    for (int i=0; i<3; i++)
    {
         if (combat_area[COMPUTER_PLAYER][i]!=255)
         {
                  num_chars ++;
                  int char_hp = get_hp(COMPUTER_PLAYER,i);
                  
                  if ((char_hp<weakest_hp||!weakest_char)&&can_attack(COMPUTER_PLAYER,i))
                  {
                       weakest_hp = char_hp;
                       weakest_char = i+1;
                  }
         }
    }
    
    int max_hp = 0;
    int max_hp_char = 0;
    
    for (int i = 0; i<MAX_HAND_SIZE; i++)
    {
        int card_type = hand_card_type[COMPUTER_PLAYER][i];
        if (hand_cards[COMPUTER_PLAYER][i]!=255&&card_type!=ACTION_CARD)
        {
                int card_hp = 0;
                
                if (card_type == SNK_CARD)
                {
                     card_hp = SNK_chars_hp[hand_cards[COMPUTER_PLAYER][i]];
                }
                else
                {
                     card_hp = Capcom_chars_hp[hand_cards[COMPUTER_PLAYER][i]];
                }
                
                if (card_hp>max_hp)
                {
                     max_hp = card_hp;
                     max_hp_char = i;
                }
        }
    }
    
    // check if replacement card better than weakest character in
    // the combat area
    if (weakest_char&&(max_hp>weakest_hp))
    {
        make_computer_attack(weakest_char,0,0,0,0);
        card_battle(COMPUTER_PLAYER,weakest_char,human_block_pos[0]);
        
        if (player_hp[HUMAN_PLAYER])
        {
        	if (combat_area[COMPUTER_PLAYER][weakest_char-1] == 255)
        	{
                	//place_card(weakest_char-1,COMPUTER_PLAYER,max_hp_char);
                	do_computer_selects_char_to_deploy();
                	if (deploy_status[COMPUTER_PLAYER])
                	{
                		computer_deployed_card = weakest_char;
                	}
                	else
                	{
				computer_deployed_card = 0;
                	}
                	
                	//set_deploy_status(COMPUTER_PLAYER);
        	}
	}
    }
}
