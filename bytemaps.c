#include "bytemaps.h"

const char *emoji_map[EMOJI_MAP_LEN] = {
    "🌀", "🌂", "🌅", "🌈", "🌙", "🌞", "🌟", "🌠",
    "🌰", "🌱", "🌲", "🌳", "🌴", "🌵", "🌷", "🌸",
    "🌹", "🌺", "🌻", "🌼", "🌽", "🌾", "🌿", "🍀",
    "🍁", "🍂", "🍃", "🍄", "🍅", "🍆", "🍇", "🍈",
    "🍉", "🍊", "🍋", "🍌", "🍍", "🍎", "🍏", "🍐",
    "🍑", "🍒", "🍓", "🍔", "🍕", "🍖", "🍗", "🍘",
    "🍜", "🍝", "🍞", "🍟", "🍠", "🍡", "🍢", "🍣",
    "🍤", "🍥", "🍦", "🍧", "🍨", "🍩", "🍪", "🍫",
    "🍬", "🍭", "🍮", "🍯", "🍰", "🍱", "🍲", "🍳",
    "🍴", "🍵", "🍶", "🍷", "🍸", "🍹", "🍺", "🍻",
    "🍼", "🎀", "🎁", "🎂", "🎃", "🎄", "🎅", "🎈",
    "🎉", "🎊", "🎋", "🎌", "🎍", "🎎", "🎏", "🎒",
    "🎓", "🎠", "🎡", "🎢", "🎣", "🎤", "🎥", "🎦",
    "🎧", "🎨", "🎩", "🎪", "🎫", "🎬", "🎭", "🎮",
    "🎯", "🎰", "🎱", "🎲", "🎳", "🎴", "🎵", "🎷",
    "🎸", "🎹", "🎺", "🎻", "🎽", "🎾", "🎿", "🏀",
    "🏁", "🏂", "🏃", "🏄", "🏆", "🏇", "🏈", "🏉",
    "🏊", "🐀", "🐁", "🐂", "🐃", "🐄", "🐅", "🐆",
    "🐇", "🐈", "🐉", "🐊", "🐋", "🐌", "🐍", "🐎",
    "🐏", "🐐", "🐑", "🐒", "🐓", "🐔", "🐕", "🐖",
    "🐗", "🐘", "🐙", "🐚", "🐛", "🐜", "🐝", "🐞",
    "🐟", "🐠", "🐡", "🐢", "🐣", "🐤", "🐥", "🐦",
    "🐧", "🐨", "🐩", "🐪", "🐫", "🐬", "🐭", "🐮",
    "🐯", "🐰", "🐱", "🐲", "🐳", "🐴", "🐵", "🐶",
    "🐷", "🐸", "🐹", "🐺", "🐻", "🐼", "🐽", "🐾",
    "👀", "👂", "👃", "👄", "👅", "👆", "👇", "👈",
    "👉", "👊", "👋", "👌", "👍", "👎", "👏", "👐",
    "👑", "👒", "👓", "👔", "👕", "👖", "👗", "👘",
    "👙", "👚", "👛", "👜", "👝", "👞", "👟", "👠",
    "👡", "👢", "👣", "👤", "👥", "👦", "👧", "👨",
    "👩", "👪", "👮", "👯", "👺", "👻", "👼", "👽",
    "👾", "👿", "💀", "💁", "💂", "💃", "💄", "💅"
};

const char *pgp_wordlist_two[PGP_WORDLIST_TWO_LEN] = {
  "aardvark", "absurd", "accrue", "acme",
  "adrift", "adult", "afflict", "ahead",
  "aimless", "Algol", "allow", "alone",
  "ammo", "ancient", "apple", "artist",
  "assume", "Athens", "atlas", "Aztec",
  "baboon", "backfield", "backward", "basalt",
  "beaming", "bedlamp", "beehive", "beeswax",
  "befriend", "Belfast", "berserk", "billiard",
  "bison", "blackjack", "blockade", "blowtorch",
  "bluebird", "bombast", "bookshelf", "brackish",
  "breadline", "breakup", "brickyard", "briefcase",
  "Burbank", "button", "buzzard", "cement",
  "chairlift", "chatter", "checkup", "chisel",
  "choking", "chopper", "Christmas", "clamshell",
  "classic", "classroom", "cleanup", "clockwork",
  "cobra", "commence", "concert", "cowbell",
  "crackdown", "cranky", "crowfoot", "crucial",
  "crumpled", "crusade", "cubic", "deadbolt",
  "deckhand", "dogsled", "dosage", "dragnet",
  "drainage", "dreadful", "drifter", "dropper",
  "drumbeat", "drunken", "Dupont", "dwelling",
  "eating", "edict", "egghead", "eightball",
  "endorse", "endow", "enlist", "erase",
  "escape", "exceed", "eyeglass", "eyetooth",
  "facial", "fallout", "flagpole", "flatfoot",
  "flytrap", "fracture", "fragile", "framework",
  "freedom", "frighten", "gazelle", "Geiger",
  "Glasgow", "glitter", "glucose", "goggles",
  "goldfish", "gremlin", "guidance", "hamlet",
  "highchair", "hockey", "hotdog", "indoors",
  "indulge", "inverse", "involve", "island",
  "Janus", "jawbone", "keyboard", "kickoff",
  "kiwi", "klaxon", "lockup", "merit",
  "minnow", "miser", "Mohawk", "mural",
  "music", "Neptune", "newborn", "nightbird",
  "obtuse", "offload", "oilfield", "optic",
  "orca", "payday", "peachy", "pheasant",
  "physique", "playhouse", "Pluto", "preclude",
  "prefer", "preshrunk", "printer", "profile",
  "prowler", "pupil", "puppy", "python",
  "quadrant", "quiver", "quota", "ragtime",
  "ratchet", "rebirth", "reform", "regain",
  "reindeer", "rematch", "repay", "retouch",
  "revenge", "reward", "rhythm", "ringbolt",
  "robust", "rocker", "ruffled", "sawdust",
  "scallion", "scenic", "scorecard", "Scotland",
  "seabird", "select", "sentence", "shadow",
  "showgirl", "skullcap", "skydive", "slingshot",
  "slothful", "slowdown", "snapline", "snapshot",
  "snowcap", "snowslide", "solo", "spaniel",
  "spearhead", "spellbind", "spheroid", "spigot",
  "spindle", "spoilage", "spyglass", "stagehand",
  "stagnate", "stairway", "standard", "stapler",
  "steamship", "stepchild", "sterling", "stockman",
  "stopwatch", "stormy", "sugar", "surmount",
  "suspense", "swelter", "tactics", "talon",
  "tapeworm", "tempest", "tiger", "tissue",
  "tonic", "tracker", "transit", "trauma",
  "treadmill", "Trojan", "trouble", "tumor",
  "tunnel", "tycoon", "umpire", "uncut",
  "unearth", "unwind", "uproot", "upset",
  "upshot", "vapor", "village", "virus",
  "Vulcan", "waffle", "wallet", "watchword",
  "wayside", "willow", "woodlark", "Zulu"
};

const char *pgp_wordlist_three[PGP_WORDLIST_THREE_LEN] = {
  "adroitness", "adviser", "aggregate", "alkali",
  "almighty", "amulet", "amusement", "antenna",
  "applicant", "Apollo", "armistice", "article",
  "asteroid", "Atlantic", "atmosphere", "autopsy",
  "Babylon", "backwater", "barbecue", "belowground",
  "bifocals", "bodyguard", "borderline", "bottomless",
  "Bradbury", "Brazilian", "breakaway", "Burlington",
  "businessman", "butterfat", "Camelot", "candidate",
  "cannonball", "Capricorn", "caravan", "caretaker",
  "celebrate", "cellulose", "certify", "chambermaid",
  "Cherokee", "Chicago", "clergyman", "coherence",
  "combustion", "commando", "company", "component",
  "concurrent", "confidence", "conformist", "congregate",
  "consensus", "consulting", "corporate", "corrosion",
  "councilman", "crossover", "cumbersome", "customer",
  "Dakota", "decadence", "December", "decimal",
  "designing", "detector", "detergent", "determine",
  "dictator", "dinosaur", "direction", "disable",
  "disbelief", "disruptive", "distortion", "divisive",
  "document", "embezzle", "enchanting", "enrollment",
  "enterprise", "equation", "equipment", "escapade",
  "Eskimo", "everyday", "examine", "existence",
  "exodus", "fascinate", "filament", "finicky",
  "forever", "fortitude", "frequency", "gadgetry",
  "Galveston", "getaway", "glossary", "gossamer",
  "graduate", "gravity", "guitarist", "hamburger",
  "Hamilton", "handiwork", "hazardous", "headwaters",
  "hemisphere", "hesitate", "hideaway", "holiness",
  "hurricane", "hydraulic", "impartial", "impetus",
  "inception", "indigo", "inertia", "infancy",
  "inferno", "informant", "insincere", "insurgent",
  "integrate", "intention", "inventive", "Istanbul",
  "Jamaica", "Jupiter", "leprosy", "letterhead",
  "liberty", "maritime", "matchmaker", "maverick",
  "Medusa", "megaton", "microscope", "microwave",
  "midsummer", "millionaire", "miracle", "misnomer",
  "molasses", "molecule", "Montana", "monument",
  "mosquito", "narrative", "nebula", "newsletter",
  "Norwegian", "October", "Ohio", "onlooker",
  "opulent", "Orlando", "outfielder", "Pacific",
  "pandemic", "pandora", "paperweight", "paragon",
  "paragraph", "paramount", "passenger", "pedigree",
  "Pegasus", "penetrate", "perceptive", "performance",
  "pharmacy", "phonetic", "photograph", "pioneer",
  "pocketful", "politeness", "positive", "potato",
  "processor", "prophecy", "provincial", "proximate",
  "puberty", "publisher", "pyramid", "quantity",
  "racketeer", "rebellion", "recipe", "recover",
  "repellent", "replica", "reproduce", "resistor",
  "responsive", "retraction", "retrieval", "retrospect",
  "revenue", "revival", "revolver", "Sahara",
  "sandalwood", "sardonic", "Saturday", "savagery",
  "scavenger", "sensation", "sociable", "souvenir",
  "specialist", "speculate", "stethoscope", "stupendous",
  "supportive", "surrender", "suspicious", "sympathy",
  "tambourine", "telephone", "therapist", "tobacco",
  "tolerance", "tomorrow", "torpedo", "tradition",
  "travesty", "trombonist", "truncated", "typewriter",
  "ultimate", "undaunted", "underfoot", "unicorn",
  "unify", "universe", "unravel", "upcoming",
  "vacancy", "vagabond", "versatile", "vertigo",
  "Virginia", "visitor", "vocalist", "voyager",
  "warranty", "Waterloo", "whimsical", "Wichita",
  "Wilmington", "Wyoming", "yesteryear", "Yucatan"
};


mapping_t a2mapping_t(char *map_name) {
  mapping_t map;
  if (strlen(map_name) >=3 && strncmp(map_name, "hex", 1) == 0) {
    map = HEX;
  }
  else if (strlen(map_name) >=5 && strncmp(map_name, "emoji", 1) == 0) {
    map = EMOJI;
  }
  else {
    fprintf(stderr, "No such bytemap: %s\n", map_name);
    exit(-1);
  }
  return map;
}

void get_display_hash(unsigned char *hash, size_t hash_len, mapping_t mapping, char **outstring) {
  char *os;
  switch (mapping) {
    case HEX:
      os = buf2hex(hash, hash_len); break;
    case EMOJI:
      os = buf2emoji(hash, hash_len); break;
    default:
      fprintf(stderr, 
              "ERROR: mapping is set to %i but I can't tell what that means.\n",
              mapping);
      os = ""; break;
  }
  *outstring = os;
  /* TODO: free the 'os' memory how? */
}

/**
 * Map the bytes in a buffer to hex values
 * 
 * @param buffer a buffer
 * @param buflen the length of the buffer in bytes
 * @return a string of hex digits seperated by colons such as 'a5:00:26:1e'
 */
char *buf2hex(unsigned char *buffer, size_t buflen) {
  char hexbyterep[3];
  int inctr;
  unsigned int singlebyte;
  char *os="";

  for (inctr=0; inctr<buflen; inctr++) {
    singlebyte = (unsigned int)buffer[inctr];
    sprintf(hexbyterep, "%02x", buffer[inctr]);
    os = concat_bytearray(os, hexbyterep);
    if (inctr != buflen-1) {
      os = concat_bytearray(os, ":");
    }
  }
  return os;
  //TODO: how to free this memory?
}

/**
 * Map the bytes in a buffer to emoji
 *
 * The list of emoji was originally taken from 
 * <http://www.windytan.com/2014/10/visualizing-hex-bytes-with-unicode-emoji.html>
 * 
 * @param buffer a buffer
 * @param buflen the length of the buffer in bytes
 * @return a string of emoji (punctuated by spaces and colons) that represents bytes in the buffer. in the form of '🐜 :🌀 :🍏 :🍇 '
 */
char *buf2emoji(unsigned char *buffer, size_t buflen) {
  char *emojibyterep;
  int inctr;
  unsigned int singlebyte;
  char *os="";

  for (inctr=0; inctr<buflen; inctr++) {
    singlebyte = (unsigned int)buffer[inctr];
    emojibyterep = (char *)emoji_map[singlebyte];

    os = concat_bytearray(os, emojibyterep);

    // Add a space between each emoji b/c they will overlap otherwise
    // Add a colon after each emoji except the last one
    if (inctr != buflen-1) {
      os = concat_bytearray(os, " :");
    }
    else {
      os = concat_bytearray(os, " ");
    }
  }
  return os; 
  // TODO: how to free this memory? 
}

