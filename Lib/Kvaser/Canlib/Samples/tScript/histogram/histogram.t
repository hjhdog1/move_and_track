// This script listens to CAN messages defined in histogram.dbc.
//
// This script should be loaded on slot 0
// E.g. by issuing the command
// tutil.exe -channel=0 -slot=0 -load histogram.txe -start -listen -1
//
// The resulting file may later be read with the command
// tutil.exe -channel=0 -copyfrom hist.mat hist.mat
//
// A presentation of the result can then be done from Matlab/Octave by
// running the hist_cmd.m file (from within Matlab/Ocatve)

variables {
  const char time_stamp[] = "Time-stamp: <2012-11-19 08:46:16 extmc>\n";

  const char filename[] = "hist.mat";

  const int NUM_SLOTS = 10;
  const int NUM_LOAD_SLOTS  = NUM_SLOTS;
  const int NUM_SPEED_SLOTS = NUM_SLOTS;
  const int NUM_TEMP_SLOTS  = NUM_SLOTS;
  const int NUM_FUEL_SLOTS  = NUM_SLOTS;

  const int MAX_INT = 0x7fffffff;

  const int sample_timeout =  1000; // Sample rate 1/1000 ms = 1/s
  const int save_timeout   = 60000; // Saving rate 1/min

  const int can_bitrate = canBITRATE_1M;

  const int False = 0;
  const int True  = 1;

  // workaround for compiler restriction
  //const onOff[] = "on\0off";
  char onOff[7] = "on_off";

  typedef struct {
    float min;
    float max;
    int num_slots;
    int cur_slot;
    int num_data;
    int num_data_ovflw;
    int ovflw;
    int num_too_big;
    int num_too_small;
  } rx_ctrl_t;

  typedef struct {
    int temp[NUM_TEMP_SLOTS];
    int fuel[NUM_FUEL_SLOTS];
  } speed_data_t;

  typedef struct {
    speed_data_t speed[NUM_SPEED_SLOTS];
  } load_data_t;

  typedef struct {
    rx_ctrl_t speed_ctrl;
    rx_ctrl_t load_ctrl;
    rx_ctrl_t temp_ctrl;
    rx_ctrl_t fuel_ctrl;
    load_data_t load[NUM_LOAD_SLOTS];
  } hist_data_t;

  hist_data_t db;

  Timer sampleTimer;
  Timer saveTimer;
}

void noteValue(rx_ctrl_t rx_ctrl, float value, int debug)
{
  if (value < rx_ctrl.min) {
    rx_ctrl.num_too_small++;
    printf("value too small: %f\n", value);
    return;
  }

  if (value > rx_ctrl.max) {
    rx_ctrl.num_too_big++;
    printf("value too big: %f\n", value);
    return;
  }

  value = value - rx_ctrl.min;
  float slot_size = (rx_ctrl.max - rx_ctrl.min) / rx_ctrl.num_slots;
  int slot = value / slot_size;

  if (slot > rx_ctrl.num_slots - 1) { // The algorithm is asymetric
    slot = rx_ctrl.num_slots - 1;
  }
  rx_ctrl.cur_slot = slot;
  if (rx_ctrl.num_data < MAX_INT) {
    rx_ctrl.num_data++;
  } else {
    rx_ctrl.num_data_ovflw = 1;
  }

  if (debug) {
    printf("Value %f - slot %d (%d) range: %f - %f\n", value, slot, 
           rx_ctrl.num_data, rx_ctrl.min, rx_ctrl.max);
  }
}

void rxInit (rx_ctrl_t rx_ctrl, float min, float max, int num_slots)
{
  rx_ctrl.min            = min;
  rx_ctrl.max            = max;
  rx_ctrl.num_slots      = num_slots;
  rx_ctrl.cur_slot       = -1;
  rx_ctrl.num_data       = 0;
  rx_ctrl.num_data_ovflw = 0;
  rx_ctrl.ovflw          = 0;
  rx_ctrl.num_too_big    = 0;
  rx_ctrl.num_too_small  = 0;
}

// Check that we have valid indexes and save current values
void recordSample(hist_data_t db)
{
  int speed = db.speed_ctrl.cur_slot;
  int load  = db.load_ctrl.cur_slot;
  int temp  = db.temp_ctrl.cur_slot;
  int fuel  = db.fuel_ctrl.cur_slot;

  char temp_txt[20];
  char fuel_txt[20];

  // have we noted both a speed and a load value?
  if (speed >= 0 && load >= 0) {
    // have we noted a valid value for temp?
    if (temp >= 0) {
      // check overflow
      if (db.load[load].speed[speed].temp[temp] < MAX_INT) {
        db.load[load].speed[speed].temp[temp]++;
      } else {
        db.speed_ctrl.ovflw = 1;
      }
      sprintf(temp_txt, "%d", db.load[load].speed[speed].temp[temp]);
    } else {
      temp_txt = "-";
    }
    // have we noted a value for fuel?
    if (fuel >= 0) {
      // check overflow
      if (db.load[load].speed[speed].fuel[fuel] < MAX_INT) {
        db.load[load].speed[speed].fuel[fuel]++;
      } else {
        db.fuel_ctrl.ovflw = 1;
      }
      sprintf(fuel_txt, "%d", db.load[load].speed[speed].fuel[fuel]);
    } else {
      fuel_txt = "-";
    }
    printf("Sample (%d,%d): temp(%d):%s, fuel(%d):%s\n", load, speed,
           temp, temp_txt, fuel, fuel_txt);
  } else {
    printf("No valid base coordinates (s:%d, l:%d)\n", speed, load);
  }
}

void readFile(const char filename[])
{
  int len = 1;
  char txt[200];
  FileHandle fileh;

  int res = fileOpen(fileh, filename, OPEN_READ);
  if (res < 0) {
      printf("ERROR: fileOpen('%s', OPEN_READ), res=%d\n", filename, res);
      return;
  }
  printf("--- %s ---\n", filename);
  while (len > 0) {
    len = fileGets(fileh, txt, txt.count);
    printf("%s", txt);
  }

  fileClose(fileh);
}

void printTemp(hist_data_t db, FileHandle fileh, int use_file) 
{
  char txt[500];
  int  len = 0;

  for (int load = 0; load < db.load.count; load++) {
    for (int speed = 0; speed < db.load[load].speed.count; speed ++) {
      if (!use_file) {
        len += sprintf(txt + len, "(%2d,%2d) ", speed, load);
      }
      for (int temp = 0; temp < db.load[load].speed[speed].temp.count; temp++) {
        if (!use_file) {
          len += sprintf(txt + len, "%3d ", db.load[load].speed[speed].temp[temp]);
        } else {
          len += sprintf(txt + len, " %3d\n", db.load[load].speed[speed].temp[temp]);
        }
      }
      if (use_file) {
        sprintf(txt, "%s", txt);
        filePuts(fileh, txt);
      } else {
        printf("%s\n", txt);
      }
      len = 0;
    }
  }
}

void printFuel(hist_data_t db, FileHandle fileh, int use_file) 
{
  char txt[500];
  int len = 0;
  for (int load = 0; load < db.load.count; load++) {
    for (int speed = 0; speed < db.load[load].speed.count; speed ++) {
      if (!use_file) {
        len += sprintf(txt + len, "(%2d,%2d) ", speed, load);
      }
      for (int fuel = 0; fuel < db.load[load].speed[speed].fuel.count; fuel++) {
        if (!use_file) {
          len += sprintf(txt + len, "%3d ", db.load[load].speed[speed].fuel[fuel]);
        } else {
          len += sprintf(txt + len, " %3d\n", db.load[load].speed[speed].fuel[fuel]);
        }
      }
      if (use_file) {
        sprintf(txt, "%s", txt);
        filePuts(fileh, txt);
      } else {
        printf("%s\n", txt);
      }
      len = 0;
    }
  }
}

void rxDump(const char txt[], rx_ctrl_t rx_ctrl)
{
  float slot_size = (rx_ctrl.max - rx_ctrl.min) / rx_ctrl.num_slots;
  printf("%5s: num:%d (%d), big:%d, small:%d, ovfl: %d, range: %f - %f, slot_size:%f\n", 
         txt,
         rx_ctrl.num_data,
         rx_ctrl.num_data_ovflw,
         rx_ctrl.num_too_big,
         rx_ctrl.num_too_small,
         rx_ctrl.ovflw,
         rx_ctrl.min,
         rx_ctrl.max,
         slot_size
         );
}

void printInfo(hist_data_t db)
{
  printf("Received the following number of data:\n");
  rxDump("Speed", db.speed_ctrl);
  rxDump("Load", db.load_ctrl);
  rxDump("Temp", db.temp_ctrl);
  rxDump("Fuel", db.fuel_ctrl);
}

void printData(hist_data_t db)
{
  FileHandle null_file;
  printf("--- TEMP ---\n");
  printTemp(db, null_file, False);
  printf("--- FUEL ---\n");
  printFuel(db, null_file, False);
  printf("\n");
  printInfo(db);
}

void saveFile(hist_data_t db, const char filename[])
{
  char txt[500];
  tm now;
  FileHandle fileh;

  printf("Saving data to file %s\n", filename);
  int res = fileOpen(fileh, filename, OPEN_TRUNCATE);
  if (res < 0) {
      printf("ERROR: fileOpen('%s', OPEN_TRUNCATE), res=%d\n", filename, res);
      return;
  }
  
  char head[10] = "TEMP";

  timeGetDate(now);
  sprintf(txt, "# Created by sample histogram.t, %d-%02d-%02d %02d:%02d:%02d\n",
          now.tm_year + 1900, now.tm_mon, now.tm_mday, 
          now.tm_hour, now.tm_min, now.tm_sec);
  filePuts(fileh, txt);

  sprintf(txt, "# name: TEMP\n# type: matrix\n# ndims: 3\n");
  filePuts(fileh, txt);
  sprintf(txt, "%d %d %d\n", NUM_LOAD_SLOTS, NUM_SPEED_SLOTS, NUM_TEMP_SLOTS);
  filePuts(fileh, txt);
  printTemp(db, fileh, True);
  filePuts(fileh, "\n\n");

  sprintf(txt, "# name: FUEL\n# type: matrix\n# ndims: 3\n");
  filePuts(fileh, txt);
  sprintf(txt, "%d %d %d\n", NUM_LOAD_SLOTS, NUM_SPEED_SLOTS, NUM_TEMP_SLOTS);
  filePuts(fileh, txt);
  printFuel(db, fileh, True);

  fileClose(fileh);
}

void dbClear(hist_data_t db) {
  for (int load = 0; load < db.load.count; load++) {
    for (int speed = 0; speed < db.load[load].speed.count; speed ++) {
      db.load[load].speed[speed].temp = 0;
      db.load[load].speed[speed].fuel = 0;
    }
  }
}

// toggle sampling on/off
int sampleToggle(Timer tim)
{
  int ms = timerIsPending(tim);
  if (ms) {
    timerCancel(tim);
    return 3;  // pointing at off in onoff string
  } else {
    timerStart(tim, FOREVER);
    return 0;
  }
}

on start {
  printf("Starting histogram program.\n");
  
  rxInit(db.load_ctrl,   0.0,  100.0, db.load.count);
  rxInit(db.speed_ctrl,  0.0, 6000.0, db.load[0].speed.count);
  rxInit(db.temp_ctrl, -60.0,  200.0, db.load[0].speed[0].temp.count);
  rxInit(db.fuel_ctrl,   0.0,  300.0, db.load[0].speed[0].fuel.count);

  dbClear(db);
  //Here we could initialize the db by reading any existing file from sd card.

  canSetBitrate(can_bitrate);
  canSetBusOutputControl(canDRIVER_NORMAL);
  canBusOn();

  sampleTimer.timeout = sample_timeout;
  saveTimer.timeout   = save_timeout;

  timerStart(sampleTimer, FOREVER);
  timerStart(saveTimer, FOREVER);

  // workaround for compiler restriction
  onOff[2] = '\0';
}

on stop {
  int i;
  printf("Stopping histogram program.\n");
  timerCancel(sampleTimer);
  canBusOff();

  saveFile(db, filename);
  printf("File saved.\n");

  printInfo(db);
}

on Timer sampleTimer {
  recordSample(db);
}

on Timer saveTimer {
  saveFile(db, filename);
  printf("File saved.\n");
}

on CanMessage ECM_001 {
  noteValue(db.speed_ctrl, (float) this.EngineSpeed.Phys, False);
  //printf("ECM_001 %d\n", this.EngineSpeed.Phys);
}

on CanMessage LIM_002 {
  noteValue(db.load_ctrl, this.Load.Phys, False);
  // printf("LIM_002 %f\n", this.Load.Phys);
}

on CanMessage ECM_003 {
  noteValue(db.temp_ctrl, this.EngineTemp.Phys, False);
  // printf("ECM_003 %f\n", this.EngineTemp.Phys);
}

on CanMessage ECM_004 {
  noteValue(db.fuel_ctrl, this.Fuel.Phys, False);
  // printf("ECM_004 %f\n", this.Fuel.Phys);
}

on key 'f' {
  saveFile(db, filename);
}

on key 'd' {
  printInfo(db);
}

on key 'r' {
  readFile(filename);
}

on key 's' {
  printf("Sampling is now %s.\n", onOff + sampleToggle(sampleTimer));
}

on key 'p' {
  printData(db);
}

on key 'q' {
  printf("Stopping histogram stimuli.\n");
  scriptStop(1); // slot 1
  printf("Stopping slot 0.\n");
  scriptStop(0); // slot 0
}

on key '?' {
  printf("\n Histogram program\n");
  printf("=====================\n");
  printf("%s", time_stamp);
  printf(" ? - Print this help\n");
  printf(" d - print db summary\n");
  printf(" f - save db to file %s\n", filename);
  printf(" p - print db contents\n");
  printf(" r - read file %s\n", filename);
  printf(" s - Toggle sampling (current %d)\n", timerIsPending(sampleTimer));
  printf(" q - Quit\n");
}
