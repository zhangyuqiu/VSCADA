create table if not exists system_info(
starttime char not null,
endtime char not null
);
create table if not exists system_log(
time char not null,
responseid char not null,
message char not null
);
create table if not exists sensors(
sensorindex char not null,
sensorname char not null,
subsystem char not null,
minthreshold char not null,
maxthreshold char not null,
maxresponseid char not null,
minresponseid char not null,
calconstant char not null
);
create table if not exists TSV_rawdata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists COOLING_rawdata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists TSI_rawdata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists GLV_rawdata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists TSV_caldata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists COOLING_caldata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists TSI_caldata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
create table if not exists GLV_caldata(
time char not null,
sensorindex char not null,
sensorname char not null,
value char not null
);
