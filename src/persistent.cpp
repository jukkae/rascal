#include "main.hpp"

void Actor::save(TCODZip& zip) {                                                                              
    zip.putInt(x);                                                                                            
    zip.putInt(y);                                                                                            
    zip.putInt(ch);                                                                                           
    zip.putColor(&col); // TODO saving or loading of color seems to be buggy - look into LibCOTD              
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
