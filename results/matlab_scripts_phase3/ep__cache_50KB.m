%% Import the data
[~, ~, raw] = xlsread('../results_parsers_phase2/Cache/Results-herald-epidemic-random-mob-netpress-134/epidemic-random-mob-netpress-134-CI.xlsx','Sheet1');
raw = raw(2:end,:);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
ep_50KB_Loveddelratio = data(:,1);
ep_50KB_Loveddeldelay = data(:,2);
ep_50KB_Nonloveddelratio = data(:,3);
ep_50KB_Nonloveddeldelay = data(:,4);
ep_50KB_Totalbytessent = data(:,5);

%% Clear temporary variables
clear data raw;
