create table if not exists system_log(
time char not null,
reactionId char not null,
message char not null
);
create table if not exists sensors(
sensorIndex char not null,
sensorName char not null,
subsystem char not null,
minThreshold char not null,
maxThreshold char not null,
maxReactionId char not null,
minReactionId char not null
);
create table if not exists reactions(
reactionId char not null,
message char not null
);
create table if not exists system_info(
runId char not null,
startTime char not null,
endTime char not null
);
create table if not exists TSV_rawdata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists COOLING_rawdata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists TSI_rawdata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists GLV_rawdata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists TSV_caldata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists COOLING_caldata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists TSI_caldata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
create table if not exists GLV_caldata(
time char not null,
sensorindex char not null,
sensorName char not null,
value char not null
);
