%% Import the data
[~, ~, raw] = xlsread('../results_parsers_phase2/Cache/Results-epidemic-random-mob-500-nodes/epidemic-random-mob-500-nodes-CI.xlsx','Sheet1');
raw = raw(2:end,:);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
ep_5MB_Loveddelratio = data(:,1);
ep_5MB_Loveddeldelay = data(:,2);
ep_5MB_Nonloveddelratio = data(:,3);
ep_5MB_Nonloveddeldelay = data(:,4);
ep_5MB_Totalbytessent = data(:,5);

%% Clear temporary variables
clear data raw;
