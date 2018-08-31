%% Import the data
[~, ~, raw] = xlsread('../results_parsers_phase2/Cache/Results-herald-epidemic-random-mob-cache-1MB/epidemic-random-mob-cache-1MB-CI.xlsx','Sheet1');
raw = raw(2:end,:);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
ep_1MB_Loveddelratio = data(:,1);
ep_1MB_Loveddeldelay = data(:,2);
ep_1MB_Nonloveddelratio = data(:,3);
ep_1MB_Nonloveddeldelay = data(:,4);
ep_1MB_Totalbytessent = data(:,5);

%% Clear temporary variables
clear data raw;
