%% Import the data
[~, ~, raw] = xlsread('../results_parsers_phase2/Nodes/Results-epidemic-random-mob-1250-nodes/epidemic-random-mob-1250-nodes-CI.xlsx','Sheet1');
raw = raw(2:end,:);

%% Create output variable
data = reshape([raw{:}],size(raw));

%% Allocate imported array to column variable names
ep_nodes_1250_Loveddelratio = data(:,1);
ep_nodes_1250_Loveddeldelay = data(:,2);
ep_nodes_1250_Nonloveddelratio = data(:,3);
ep_nodes_1250_Nonloveddeldelay = data(:,4);
ep_nodes_1250_Totalbytessent = data(:,5);

%% Clear temporary variables
clear data raw;
