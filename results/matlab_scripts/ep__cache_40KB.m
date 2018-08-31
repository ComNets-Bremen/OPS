%% Import the data
[~, ~, raw] = xlsread('../results_parsers_phase2/Cache/Results-herald-epidemic-random-mob-netpress-168/epidemic-random-mob-netpress-168-CI.xlsx','Sheet1');
raw = raw(2:end,:);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
ep_40KB_Loveddelratio = data(:,1);
ep_40KB_Loveddeldelay = data(:,2);
ep_40KB_Nonloveddelratio = data(:,3);
ep_40KB_Nonloveddeldelay = data(:,4);
ep_40KB_Totalbytessent = data(:,5);

%% Clear temporary variables
clear data raw;
