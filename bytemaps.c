#include "bytemaps.h"

char *valid_value_separators = ", \0";

/*
 * The list of emoji was originally taken from 
 * <http://www.windytan.com/2014/10/visualizing-hex-bytes-with-unicode-emoji.html>
 */
const char *emoji_map[] = {
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

const char *pgp_wordlist_two[] = {
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

const char *pgp_wordlist_three[] = {
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

// Yeah I know this is dumb
const char *hex_map[] = {
  "00", "01", "02", "03", "04", "05", "06", "07", 
  "08", "09", "0a", "0b", "0c", "0d", "0e", "0f", 
  "10", "11", "12", "13", "14", "15", "16", "17", 
  "18", "19", "1a", "1b", "1c", "1d", "1e", "1f", 
  "20", "21", "22", "23", "24", "25", "26", "27", 
  "28", "29", "2a", "2b", "2c", "2d", "2e", "2f", 
  "30", "31", "32", "33", "34", "35", "36", "37", 
  "38", "39", "3a", "3b", "3c", "3d", "3e", "3f", 
  "40", "41", "42", "43", "44", "45", "46", "47", 
  "48", "49", "4a", "4b", "4c", "4d", "4e", "4f", 
  "50", "51", "52", "53", "54", "55", "56", "57", 
  "58", "59", "5a", "5b", "5c", "5d", "5e", "5f", 
  "60", "61", "62", "63", "64", "65", "66", "67", 
  "68", "69", "6a", "6b", "6c", "6d", "6e", "6f", 
  "70", "71", "72", "73", "74", "75", "76", "77", 
  "78", "79", "7a", "7b", "7c", "7d", "7e", "7f", 
  "80", "81", "82", "83", "84", "85", "86", "87", 
  "88", "89", "8a", "8b", "8c", "8d", "8e", "8f", 
  "90", "91", "92", "93", "94", "95", "96", "97", 
  "98", "99", "9a", "9b", "9c", "9d", "9e", "9f", 
  "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", 
  "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af", 
  "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", 
  "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf", 
  "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", 
  "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf", 
  "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", 
  "d8", "d9", "da", "db", "dc", "dd", "de", "df", 
  "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", 
  "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef", 
  "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", 
  "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
};

mapping_t a2mapping_t(char *map_name) {
  mapping_t map;
  if (strlen(map_name) >=3 && strncmp(map_name, "hex", 3) == 0) {
    map = HEX;
  }
  else if (strlen(map_name) >=5 && strncmp(map_name, "emoji", 3) == 0) {
    map = EMOJI;
  }
  else if (strlen(map_name) >=3 && strncmp(map_name, "pgp", 3) == 0) {
    map = PGP;
  }
  else {
    dbgprintf("No such bytemap: %s\n", map_name);
    return NOMAPPING;
  }
  return map;
}


composedmap_type * a2composedmap_type(
  const char *name,
  configuration_type *config) 
{
  int ix;
  for (ix=0; ix< config->composedmaps_count; ix++) {
    if (safe_strcmp(config->composedmaps[ix]->name, name)) {
      return config->composedmaps[ix];
    }
  }
  return NULL;
}

rawmap_type * a2rawmap_type(
  const char *name,
  configuration_type *config) 
{
  int ix;
  for (ix=0; ix< config->rawmaps_count; ix++) {
    if (safe_strcmp(config->rawmaps[ix]->name, name)) {
      return config->rawmaps[ix];
    }
  }
  return NULL;
}





/* Callback function for inih library. 
 * 
 * Called once per line of the config file.
 * 
 * nonreentrant: uses strtok(), static local vars
 *
 * TODO: 
 * - error checking?  what's that? 
 * 
 * @param configuration a pointer to a struct that will contain our config data
 * @param section the name of the current section 
 * @param name the name of the configuration item (the part to the left of the
 *        equals sign)
 * @param value the value of the configuration item (the part to the right of
 *        the equals sign)
 */
int inih_handler(
  void *configuration, 
  const char *section, 
  const char *name,
  const char *value)
{

  configuration_type *pconfig = (configuration_type*) configuration;

  // static local variables are initialized to 0
  static int rmcount, cmcount, bytectr; 
  rawmap_type *current_rawmap;
  composedmap_type *current_composedmap;
  char *token, *cmname;

  int ix;

  // value2 exists b/c strtok() modifies the string passed to it; we need to 
  // copy 'value' into 'value2' so that the INI parsing library doesn't barf
  char *value2; 
  value2 = malloc(strlen(value) +1);  
  memcpy(value2, value, strlen(value) +1);


  /* This will print out each line of the config file as its being parsed:
  static int linectr;
  linectr +=1;
  dbgprintf("inih_handler(): "
    "%s (%zi) // %s (%zi) // %s (%zi). byte: %i // line: %i \n", 
    section, strlen(section), name, strlen(name), value, strlen(value), 
    bytectr, linectr);
  */

  if (strlen(section) == 0) {
    dbgprintf("inih_handler(): in general section\n");
  }

  if (safe_strcmp(section, "rawmaps")) {

    current_rawmap = a2rawmap_type(name, pconfig);
    if (!current_rawmap) {
      bytectr = 0;
      rmcount +=1;
      pconfig->rawmaps_count = rmcount;
      pconfig->rawmaps = realloc(pconfig->rawmaps, 
        sizeof(rawmap_type*) * rmcount);

      current_rawmap = malloc(sizeof(rawmap_type));
      current_rawmap->name = strdup(name);
      pconfig->rawmaps[rmcount-1] = current_rawmap;
    }

    token = strtok( (char*)value2, valid_value_separators);
    //dbgprintf("bytectr: \n");
    for (; token != NULL; bytectr++) {
      //dbgprintf("  %i: '%s'\n", bytectr, token);
      current_rawmap->map[bytectr] = token;
      token = strtok(NULL, valid_value_separators);
    }

    if (bytectr == 256) { 
    }
    else if (bytectr > 256) {
      dbgprintf("inih_handler(): more than 256 values for raw map. "
        "Current bytectr == %i\n", bytectr);
      return 0;
    }

  }

  else if (safe_strncmp(section, "composedmap", strlen("composedmap"))) {

    cmname = (char*) &(section[ strlen("composedmap ") ]);

    current_composedmap = a2composedmap_type(cmname, pconfig);
    if (!current_composedmap) {

      cmcount +=1;
      pconfig->composedmaps_count = cmcount;
      pconfig->composedmaps = realloc(pconfig->composedmaps,
        sizeof(composedmap_type*) * cmcount);

      current_composedmap = malloc(sizeof(composedmap_type));
      current_composedmap->name = strdup(cmname);
      pconfig->composedmaps[cmcount-1] = current_composedmap;
    }

    if (safe_strcmp(name, "rawmaps")) {

      token = strtok( (char*)value2, valid_value_separators);
      for (ix=0; token != NULL; ix++) {

        current_rawmap = a2rawmap_type(token, pconfig);
        current_composedmap->rawmaps_count +=1;
        current_composedmap->rawmaps = realloc(current_composedmap->rawmaps,
          sizeof(rawmap_type*) * current_composedmap->rawmaps_count);
        current_composedmap->rawmaps[ix] = current_rawmap;

        token = strtok(NULL, valid_value_separators);
      }
    }

    else if (safe_strcmp(name, "separator") || 
      safe_strcmp(name, "terminator")) 
    {
      if (value2[0] == '"') {
        value2 = &value2[1];
      }
      if (value2[ strlen(value2) -1 ] == '"') {
        value2[ strlen(value2) -1 ] = '\0';
      }

      if (safe_strcmp(name, "separator")) {
        current_composedmap->separator = strdup(value2);
      }
      else if (safe_strcmp(name, "terminator")) {
        current_composedmap->terminator = strdup(value2);
      }
    }

    else {
      dbgprintf("Ignoring extra field in config file: '%s'\n", name);
    }

  }

  else {
    dbgprintf("inih_handler(): Unknown section: '%s'\n", section);
    return 0;
  }

  return 1;
}





// TODO: deal with Unicode. These functions will work with ASCII and UTF-8, but
// nothing else


/* Transform a hex string to a byte array that the hex string represents
 * 
 * @param hexstring a string of the form '0x12ab...' or '12:ab:...' or 
 *        '12ab...'
 *
 * @param outbuffer a pointer which will be set to the byte array
 * 
 * @return the length of the outbuffer if successful, a negative number otherwise
 */
int hex2buf(char * hexstring, unsigned char ** outbuffer) {
  char *normhs;
  int outbuffer_len;

  normhs = normalize_hexstring(hexstring);
  if (!normhs) {
    fprintf(stderr, "The string %s is not a hex string\n", hexstring);
    return -1;
  }

  *outbuffer = nhex2int(normhs);
  if (! *outbuffer) {
    fprintf(stderr, "The string %s is not a hex string\n", hexstring);
    return -1;
  }

  outbuffer_len = strlen(normhs)/2;

  dbgprintf("hex2buf(): Reconstructed hex representation of input string: "
    "'%s'\n", get_display_hash(*outbuffer, outbuffer_len, HEX));

  free(normhs);
  return outbuffer_len;
}

/* Convert a hit (a _H_ex dig_IT_) to an integer 
 * (used in nhex2int())
 *
 * Cribbed from: http://stackoverflow.com/questions/12535320/
 * See also: http://stackoverflow.com/questions/5089701/
 * 
 * @param hit a hex digit, in the class [0-9a-f]. Uppercase letters are not
 *        supported.
 * 
 * @return an integer value between 0-15.
 *
 * TODO: why does use of the `inline` keyword produce no errors on Mac OS X but 
 *       fail to compile on Linux? 
 * TODO: make this work with upper case hits as well
 * TODO: what happens if you pass bad data here? 
 */
//inline int hit2int(char hit) {
int hit2int(char hit) {
  if (! (('0'<=hit<='9') || ('a'<=hit<='f'))) {
    fprintf(stderr, "Bad argument to hit2int(): '%c'\n", hit);
    return -1;
  }
  return ((hit) <= '9' ? (hit) - '0' : (hit) - 'a' + 10);
}

/* Normalize a string containing a hexadecimal representation of a number
 * 
 * @param hexstring a string containing a hex representation of a number
 *
 * @return a normalized, guaranteed lower-case, null-terminated hexstring 
 *         without leading '0x' or inter-byte ':' characters. if the hexstring
 *         is invalid, return NULL. the returned pointer must be freed by the
 *         caller.
 */
char * normalize_hexstring(char * hexstring) {
  unsigned int iidx=0, oidx=0;
  char ca, cb;
  char * hexstring_norm = (char *) malloc(sizeof(char) * strlen(hexstring) +1);

  if (hexstring[0] == '0' && hexstring[1] == 'x') {
    iidx = 2;
  }

  while (iidx < strlen(hexstring)) {

    ca = hexstring[iidx];
    cb = hexstring[iidx +1];

    if ('A'<=ca<='F') ca = tolower(ca);
    if ('A'<=cb<='F') cb = tolower(cb);

    if (hit2int(ca) <0 || hit2int(cb) <0) {
      fprintf(stderr, "normalize_hexstring(): bad input\n");
      return NULL;
    }

    dbgprintf("normalize_hexstring(): ca = %c, bc = %c\n", ca, cb);

    if (ca != ':') {
      hexstring_norm[oidx] = ca;
      hexstring_norm[oidx+1] = cb;
      oidx += 2;
      iidx += 2;
    }
    else {
      iidx++;
    }
  }
  dbgprintf("Original hexstring: '%s'; normalized: '%s'\n", hexstring, (char*)hexstring_norm);

  return hexstring_norm;
}

/* Convert a normalized hex string to a byte array that the hex string 
 * represents
 * 
 * @param hexstring a string with any number of pairs of hex digits, without 
 *        leading '0x' or inter-byte ':' characters
 * 
 * @return a buffer strlen(hexstring)/2 bytes long containing the byte array
 *         represented by hexstring
 */
unsigned char * nhex2int(char * hexstring) {
  size_t buffer_len = strlen(hexstring) / 2;
  unsigned char * buffer = malloc(buffer_len);
  long ix;
  int ia, ib;

  for (ix=0; ix<buffer_len; ix++) {
    ia = hit2int(hexstring[2 * ix + 0]);
    ib = hit2int(hexstring[2 * ix + 1]);
    if (ia <0 || ib <0) {
      return NULL;
    }
    // shift ia four bits to the left, because four bits is half of one byte 
    // and ia is the first half of the byte representation.
    buffer[ix] = (ia << 4) | ib;
  }

  dbgprintf("nhex2int(): Reconstructed hex representation of input string: "
    "'%s'\n", get_display_hash(buffer, buffer_len, HEX));

  return buffer;
}

char *get_display_hash(unsigned char *hash, size_t hash_len, mapping_t mapping) {
  char ***maps, *separator, *terminator, *mapped_buffer;
  size_t maps_count;

  switch (mapping) {
    case HEX:
      separator = ":";
      terminator = "";
      maps_count = 1;
      maps = malloc(sizeof(char**) * maps_count);
      maps[0] = (char**) hex_map;
      break;
    case EMOJI:
      separator = " :";
      terminator = " ";
      maps_count = 1;
      maps = malloc(sizeof(char**) * maps_count);
      maps[0] = (char**) emoji_map;
      break;
    case PGP:
      separator = ", ";
      terminator = ".";
      maps_count = 2;
      maps = malloc(sizeof(char**) * maps_count);
      maps[0] = (char**) pgp_wordlist_two;
      maps[1] = (char**) pgp_wordlist_three;
      break;
    default:
      fprintf(stderr, 
              "ERROR: mapping is set to %i but I can't tell what that means.\n",
              mapping);
      return NULL;
  }  
  mapped_buffer = buf2map(hash, hash_len, separator, terminator, maps, 
    maps_count);
  if (!mapped_buffer) {
    fprintf(stderr, "Error mapping buffer.\n");
    return NULL;
  }

  return mapped_buffer;
  // NOTE: Caller must free the returned pointer
}

/**
 * Map the bytes in a buffer to values from one or more bytemaps
 * 
 * @param buffer a buffer
 *
 * @param buflen the length of the buffer in bytes
 *
 * @param separator null-terminated string to insert between each byte representation
 *
 * @param terminator null-terminated string to insert after the last byte representation 
 *
 * @param maps an array of pointers to bytemap arrays. bytemap arrays are arrays with 256 elements, where each element is a null-terminated string.
 *
 * @param maps_count the number of maps passed
 *
 * @return a null-terminated string representing the 'buffer' parameter. each byte in the original buffer is represented by one of the bytemaps passed as the 'maps' parameter; between each pair of byte representations is the separator string, and after the representation of the last byte is the terminator string. maps are used alternatingly, in the order provided - for example, if 2 maps are passed, the first byte will be represented by its value from maps[0], the second from maps[1], the third from maps[0] again, and so on. 
 *
 */
char *
buf2map(
  unsigned char *buffer, 
  size_t buflen, 
  const char *separator, 
  const char *terminator, 
  char *** maps,
  size_t maps_count)
{

  char *byterep, *os=NULL;
  int inctr=0, ossz=0, insertpt=0, septerm_longest=0, mapnum;
  unsigned int singlebyte;

  if (maps_count == 0) {
    fprintf(stderr, "Tried to map a buffer without passing any maps.\n");
    return NULL;
  }

  if (strlen(separator) > strlen(terminator)) {
    septerm_longest = strlen(separator);
  }
  else {
    septerm_longest = strlen(terminator) +1; // +1 for the terminating \0
  }

  for (inctr=0; inctr<buflen; inctr++) {
    singlebyte = (unsigned int)buffer[inctr];

    mapnum = inctr % maps_count;
    byterep = (char*) maps[mapnum][singlebyte];

    insertpt = ossz;
    ossz += strlen(byterep) + septerm_longest;

    os = realloc(os, ossz);
    if (! os) {
      fprintf(stderr, "Could not allocate memory.\n");
      return NULL;
    }

    memcpy(os +insertpt, byterep, strlen(byterep));
    insertpt += strlen(byterep);

    if (inctr != buflen-1) {
      memcpy(os +insertpt, separator, strlen(separator));
    }
    else {
      memcpy(os +insertpt, terminator, strlen(terminator)+1); // +1 for terminating \0
    }

  }
  return os;
  // NOTE: Caller must free the returned pointer
}

