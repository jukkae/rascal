#include "main.hpp"

void Engine::save() {
    if (player->destructible->isDead()) { // Permadeath
        TCODSystem::deleteFile("game.sav");
    } else {
        TCODZip zip;

		zip.putString("RASCAL V0 SAVEGAME");

        zip.putInt(map->width);
        zip.putInt(map->height);
        map->save(zip);
		
        player->save(zip);

        zip.putInt(actors.size() - 1);
        printf("actors: %d\n", actors.size() - 1);
        for (Actor** it = actors.begin(); it != actors.end(); it++) {
            if ( *it != player ) {
                (*it)->save(zip);
            }
        }
        gui->save(zip);
        zip.saveToFile("game.sav");
    }
}

void Engine::load() {
    if(TCODSystem::fileExists("game.sav")) {
        TCODZip zip;
        zip.loadFromFile("game.sav");

		char* crap = strdup(zip.getString());
        
		int width = zip.getInt();
        int height = zip.getInt();
        map = new Map(width, height);
        map->load(zip);

        player = new Actor(0, 0, 0, NULL, TCODColor::white);
        //player->destructible = new PlayerDestructible(30, 2, "your corpse");
        //player->attacker = new Attacker(5);
        //player->ai = new PlayerAi();
        //player->container = new Container(26);
        player->load(zip);
        actors.push(player);

        int nOfActors = zip.getInt();
        printf("actors: %d\n", nOfActors);
        while (nOfActors > 0) {
            Actor* a = new Actor(0, 0, 0, NULL, TCODColor::white);
            a->load(zip);
            actors.push(a);
            nOfActors--;
        }
        gui->load(zip);
    } else {
        engine.init();
    }
} 

void Actor::save(TCODZip& zip) {                                                                              
    zip.putInt(x);                                                                                            
    zip.putInt(y);                                                                                            
    zip.putInt(ch);                                                                                           
    zip.putColor(&col);
    zip.putString(name);                                                                                      
    printf("saved actor: %s\n", name);                                                                        
    zip.putInt(blocks);                                                                                       
                                                                                                              
    zip.putInt(attacker != NULL);                                                                             
    zip.putInt(destructible != NULL);                                                                         
    zip.putInt(ai != NULL);                                                                                   
    zip.putInt(pickable != NULL);                                                                             
    zip.putInt(container != NULL);                                                                            
                                                                                                              
    if (attacker)     attacker->save(zip);                                                                    
    if (destructible) destructible->save(zip);                                                                
    if (ai)           ai->save(zip);                                                                          
    if (pickable)     pickable->save(zip);                                                                    
    if (container)    container->save(zip);                                                                   
}

void Actor::load(TCODZip& zip) {                                                                              
    x      = zip.getInt();                                                                                    
    y      = zip.getInt();                                                                                    
    ch     = zip.getInt();                                                                                    
    col    = zip.getColor();                                                                                  
    name   = strdup(zip.getString());                                                                         
    printf("loaded actor: %s\n", name);                                                                       
    blocks = zip.getInt();                                                                                    
                                                                                                              
    bool hasAttacker     = zip.getInt();                                                                      
    bool hasDestructible = zip.getInt();                                                                      
    bool hasAi           = zip.getInt();                                                                      
    bool hasPickable     = zip.getInt();                                                                      
    bool hasContainer    = zip.getInt();                                                                      
                                                                                                              
    if ( hasAttacker ) {                                                                                      
        attacker = new Attacker(0.0f);                                                                        
        attacker->load(zip);                                                                                  
    }                                                                                                         
    if ( hasDestructible ) {                                                                                  
        destructible = Destructible::create(zip);                                                             
    }                                                                                                         
    if ( hasAi ) {                                                                                            
        ai = Ai::create(zip);                                                                                 
    }                                                                                                         
    if ( hasPickable ) {                                                                                      
        pickable = Pickable::create(zip);                                                                     
    }                                                                                                         
    if ( hasContainer ) {                                                                                     
        container = new Container(0);                                                                         
        container->load(zip);                                                                                 
    }                                                                                                         
}

void PlayerAi::load(TCODZip& zip) {
}

void PlayerAi::save(TCODZip& zip) {
    zip.putInt(PLAYER);
}

void MonsterAi::load(TCODZip& zip) {
    moveCount = zip.getInt();
}

void MonsterAi::save(TCODZip& zip) {
    zip.putInt(MONSTER);
    zip.putInt(moveCount);
}

void ConfusedMonsterAi::load(TCODZip& zip) {
    turns = zip.getInt();
    oldAi = Ai::create(zip);
}

void ConfusedMonsterAi::save(TCODZip& zip) {
    zip.putInt(CONFUSED_MONSTER);
    zip.putInt(turns);
    oldAi->save(zip);
} 

Ai* Ai::create(TCODZip& zip) {
    AiType type = (AiType) zip.getInt();
    Ai* ai = NULL;
    switch(type) {
        case PLAYER: ai = new PlayerAi(); break;
        case MONSTER: ai = new MonsterAi(); break;
        case CONFUSED_MONSTER: ai = new ConfusedMonsterAi(0, NULL); break;
    }
    ai->load(zip);
    return ai;
}

void Attacker::save(TCODZip& zip) {
    zip.putFloat(power);
}

void Attacker::load(TCODZip& zip) {
    power = zip.getFloat();
}

void Container::load(TCODZip& zip) {
    size = zip.getInt();
    int nbActors = zip.getInt();
    while ( nbActors > 0 ) {
        Actor* actor = new Actor(0, 0, 0, NULL, TCODColor::white);
        actor->load(zip);
        inventory.push(actor);
        nbActors--;
    }
}

void Container::save(TCODZip& zip) {
    zip.putInt(size);
    zip.putInt(inventory.size());
    for (Actor** it = inventory.begin(); it != inventory.end(); it++) {
        (*it)->save(zip);
    }
}

void Destructible::save(TCODZip& zip) {
    zip.putFloat(maxHp);
    zip.putFloat(hp);
    zip.putFloat(defense);
    zip.putString(corpseName);
}

void Destructible::load(TCODZip& zip) {
    maxHp = zip.getFloat();
    hp = zip.getFloat();
    defense = zip.getFloat();
    corpseName = strdup(zip.getString());
}  

void MonsterDestructible::save(TCODZip& zip) {
    zip.putInt(MONSTER);
    Destructible::save(zip);
} 

void PlayerDestructible::save(TCODZip& zip) {
    zip.putInt(PLAYER);
    Destructible::save(zip);
}

Destructible* Destructible::create(TCODZip& zip) {
    DestructibleType type = (DestructibleType) zip.getInt();
    Destructible* destructible = NULL;
    switch(type) {
        case MONSTER: destructible = new MonsterDestructible(0, 0, NULL); break;
        case PLAYER:  destructible = new PlayerDestructible (0, 0, NULL); break;
    }
    destructible->load(zip);
    return destructible;
}


void Gui::save(TCODZip& zip) {
    zip.putInt(log.size());
    for(Message** it = log.begin(); it != log.end(); it++) {
        zip.putString((*it)->text);
        zip.putColor(&(*it)->col);
    }
}

void Gui::load(TCODZip& zip) {
    int nMsgs = zip.getInt();
    while (nMsgs > 0) {
        const char* text = zip.getString();
        TCODColor col = zip.getColor();
        message(col, text);
        nMsgs--;
    }
}

void Map::save(TCODZip& zip) {
    zip.putInt(seed);
    for(int i = 0; i < width * height; i++) { zip.putInt(tiles[i].explored); }
}

void Map::load(TCODZip& zip) {
    seed = zip.getInt();
    init(false);
    for(int i = 0; i < width * height; i++) { tiles[i].explored = zip.getInt(); }
} 

void Healer::load(TCODZip& zip) {
    amount = zip.getFloat();
}

void Healer::save(TCODZip& zip) {
    zip.putInt(HEALER);
    zip.putFloat(amount);
}

void BlasterBolt::load(TCODZip& zip) {
    range = zip.getFloat();
    damage = zip.getFloat();
}

void BlasterBolt::save(TCODZip& zip) {
    zip.putInt(BLASTER_BOLT);
    zip.putFloat(range);
    zip.putFloat(damage);
}

void FragmentationGrenade::save(TCODZip& zip) {
    zip.putInt(FRAGMENTATION_GRENADE);
    zip.putFloat(range);
    zip.putFloat(damage);
}

void Confusor::load(TCODZip& zip) {
    turns = zip.getInt();
    range = zip.getFloat();
}

void Confusor::save(TCODZip& zip) {
    zip.putInt(CONFUSOR);
    zip.putInt(turns);
    zip.putFloat(range);
}

Pickable* Pickable::create(TCODZip& zip) {
    PickableType type = (PickableType) zip.getInt();
    Pickable* pickable = NULL;
    switch(type) {
        case HEALER : pickable = new Healer(0); break;
        case BLASTER_BOLT : pickable = new BlasterBolt(0,0); break;
        case CONFUSOR : pickable = new Confusor(0,0); break;
        case FRAGMENTATION_GRENADE : pickable = new FragmentationGrenade(0,0); break;
    }
    pickable->load(zip);
    return pickable;
}    
