% Calculate node statistics and graphs
% Tested with Matlab R2016a and Octave 4.4.0

clear all;

% Check if we are in Octave and include required libraries
isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;
if (isOctave)
  %% Packages io and signal are required for octave: octave-io octave-signal
  pkg load io;
  pkg load signal;
end

ep__nodes_250;
ep__nodes_500;
ep__nodes_750;
ep__nodes_1000;
ep__nodes_1250;

kee__nodes_250;
kee__nodes_500;
kee__nodes_750;
kee__nodes_1000;
kee__nodes_1250;
%kee__nodes_1500;
%kee__nodes_1750;

t = 2.576;
n = 30;
sqrn = square(n);
format = [0 0 6 3];

% loved del ratio

kee_loved_ratio_x = [250, 500, 750, 1000, 1250]%, 1500, 1750]
kee_loved_ratio_y = [mean(kee_nodes_250_Loveddelratio), mean(kee_nodes_500_Loveddelratio), mean(kee_nodes_750_Loveddelratio), mean(kee_nodes_1000_Loveddelratio), mean(kee_nodes_1250_Loveddelratio)]%, mean(kee_nodes_1500_Loveddelratio), mean(kee_nodes_1750_Loveddelratio)];
kee_loved_ratio_s = [std(kee_nodes_250_Loveddelratio), std(kee_nodes_500_Loveddelratio), std(kee_nodes_750_Loveddelratio), std(kee_nodes_1000_Loveddelratio), std(kee_nodes_1250_Loveddelratio)]%, std(kee_nodes_1500_Loveddelratio), std(kee_nodes_1750_Loveddelratio)];
kee_loved_ratio_CI = kee_loved_ratio_y + kee_loved_ratio_s.*t.*sqrn


ep_loved_ratio_x = [250, 500, 750, 1000, 1250]
ep_loved_ratio_y = [mean(ep_nodes_250_Loveddelratio), mean(ep_nodes_500_Loveddelratio), mean(ep_nodes_750_Loveddelratio), mean(ep_nodes_1000_Loveddelratio), mean(ep_nodes_1250_Loveddelratio)];
ep_loved_ratio_s = [std(ep_nodes_250_Loveddelratio), std(ep_nodes_500_Loveddelratio), std(ep_nodes_750_Loveddelratio), std(ep_nodes_1000_Loveddelratio), std(ep_nodes_1250_Loveddelratio)];
ep_loved_ratio_CI = ep_loved_ratio_y + ep_loved_ratio_s.*t.*sqrn

figure(1)
eb=errorbar(kee_loved_ratio_x, kee_loved_ratio_y, kee_loved_ratio_y-kee_loved_ratio_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_loved_ratio_x, ep_loved_ratio_y, ep_loved_ratio_y - ep_loved_ratio_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([500 500], [0 1], '-.k')
hold off
ylim([0 1])
set(gca,'FontSize',12)
l=legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'location', 'northeast')
set(l, 'FontSize', 14)
%title('Popular Delivery ratio, with 99% confidence intervals')
xlabel('Number of nodes')
ylabel('Delivery ratio (%)')
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/popular_ratio_nodes','png')

% zoom into area
figure(2)
eb=errorbar(kee_loved_ratio_x, kee_loved_ratio_y, kee_loved_ratio_y-kee_loved_ratio_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_loved_ratio_x, ep_loved_ratio_y, ep_loved_ratio_y - ep_loved_ratio_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([500 500], [0 1], '-.k')
hold off
ylim([0.9 1])
set(gca,'FontSize',12)
%legend({'Keetchi', 'Epidemic'}, 'FontSize', 14)
%title('Popular Delivery ratio, with 99% confidence intervals')
xlabel('Number of nodes')
ylabel('Delivery ratio (%)')
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/popular_ratio_nodes_zoomed','png')


%non-loved del ratio

kee_nonloved_ratio_x = [250, 500, 750, 1000, 1250]%, 1500, 1750]
kee_nonloved_ratio_y = [mean(kee_nodes_250_Nonloveddelratio), mean(kee_nodes_500_Nonloveddelratio), mean(kee_nodes_750_Nonloveddelratio), mean(kee_nodes_1000_Nonloveddelratio), mean(kee_nodes_1250_Nonloveddelratio)]%, mean(kee_nodes_1500_Nonloveddelratio), mean(kee_nodes_1750_Nonloveddelratio)];
kee_nonloved_ratio_s = [std(kee_nodes_250_Nonloveddelratio), std(kee_nodes_500_Nonloveddelratio), std(kee_nodes_750_Nonloveddelratio), std(kee_nodes_1000_Nonloveddelratio), std(kee_nodes_1250_Nonloveddelratio)]%, std(kee_nodes_1500_Nonloveddelratio), std(kee_nodes_1750_Nonloveddelratio)];
kee_nonloved_ratio_CI = kee_nonloved_ratio_y + kee_nonloved_ratio_s.*t.*sqrn

ep_nonloved_ratio_x = [250, 500, 750, 1000, 1250]
ep_nonloved_ratio_y = [mean(ep_nodes_250_Nonloveddelratio), mean(ep_nodes_500_Nonloveddelratio), mean(ep_nodes_750_Nonloveddelratio), mean(ep_nodes_1000_Nonloveddelratio), mean(ep_nodes_1250_Nonloveddelratio)];
ep_nonloved_ratio_s = [std(ep_nodes_250_Nonloveddelratio), std(ep_nodes_500_Nonloveddelratio), std(ep_nodes_750_Nonloveddelratio), std(ep_nodes_1000_Nonloveddelratio), std(ep_nodes_1250_Nonloveddelratio)];
ep_nonloved_ratio_CI = ep_nonloved_ratio_y + ep_nonloved_ratio_s.*t.*sqrn

figure(3)
eb=errorbar(kee_nonloved_ratio_x, kee_nonloved_ratio_y, kee_nonloved_ratio_y - kee_nonloved_ratio_CI , '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_nonloved_ratio_x, ep_nonloved_ratio_y, ep_nonloved_ratio_y - ep_nonloved_ratio_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([500 500], [0 1], '-.k')
hold off
ylim([0 1])
set(gca,'FontSize',12)
l=legend({'Keetchi', 'Epidemic', 'Reference scenario'})
set(l, 'FontSize', 14)
%title('Non-Popular Delivery ratio, with 99% confidence intervals')
xlabel('Number of nodes')
ylabel('Delivery ratio (%)')
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/non_popular_ratio_nodes','png')
% 
% delivery delay

kee_loved_delay_x = [250, 500, 750, 1000, 1250]%, 1500, 1750]
kee_loved_delay_y = [mean(kee_nodes_250_Loveddeldelay), mean(kee_nodes_500_Loveddeldelay), mean(kee_nodes_750_Loveddeldelay), mean(kee_nodes_1000_Loveddeldelay), mean(kee_nodes_1250_Loveddeldelay)]%, mean(kee_nodes_1500_Loveddeldelay), mean(kee_nodes_1750_Loveddeldelay)];
kee_loved_delay_s = [std(kee_nodes_250_Loveddeldelay), std(kee_nodes_500_Loveddeldelay), std(kee_nodes_750_Loveddeldelay), std(kee_nodes_1000_Loveddeldelay), std(kee_nodes_1250_Loveddeldelay)]%, std(kee_nodes_1500_Loveddeldelay), std(kee_nodes_1750_Loveddeldelay)];
kee_loved_delay_CI = kee_loved_delay_y + kee_loved_delay_s.*t.*sqrn

ep_loved_delay_x = [250, 500, 750, 1000, 1250]
ep_loved_delay_y = [mean(ep_nodes_250_Loveddeldelay), mean(ep_nodes_500_Loveddeldelay), mean(ep_nodes_750_Loveddeldelay), mean(ep_nodes_1000_Loveddeldelay), mean(ep_nodes_1250_Loveddeldelay)];
ep_loved_delay_s = [std(ep_nodes_250_Loveddeldelay), std(ep_nodes_500_Loveddeldelay), std(ep_nodes_750_Loveddeldelay), std(ep_nodes_1000_Loveddeldelay), std(ep_nodes_1250_Loveddeldelay)];
ep_loved_delay_CI = ep_loved_delay_y + ep_loved_delay_s.*t.*sqrn

figure(4)
eb=errorbar(kee_loved_delay_x, kee_loved_delay_y, kee_loved_delay_y - kee_loved_delay_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_loved_delay_x, ep_loved_delay_y, ep_loved_delay_y - ep_loved_delay_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([500 500], [0 21600], '-.k')
hold off
ylim([0 21600])
set(gca, 'YTick', [0 3600 7200 10800 14400 18000 21600])
set(gca, 'YTickLabel', {'0h','1h','2h','3h','4h','5h','6h'})
set(gca,'FontSize',12)
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize', 14)
%title('Popular Delivery delay, with 99% confidence intervals')
xlabel('Number of nodes')
ylabel('Delivery delay [hours]')
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/popular_delay_nodes','png')
% 
% non loved delivery delay

kee_nonloved_delay_x = [250, 500, 750, 1000, 1250]%, 1500, 1750]
kee_nonloved_delay_y = [mean(kee_nodes_250_Nonloveddeldelay), mean(kee_nodes_500_Nonloveddeldelay), mean(kee_nodes_750_Nonloveddeldelay), mean(kee_nodes_1000_Nonloveddeldelay), mean(kee_nodes_1250_Nonloveddeldelay)]%, mean(kee_nodes_1500_Nonloveddeldelay), mean(kee_nodes_1750_Nonloveddeldelay)];
kee_nonloved_delay_s = [std(kee_nodes_250_Nonloveddeldelay), std(kee_nodes_500_Nonloveddeldelay), std(kee_nodes_750_Nonloveddeldelay), std(kee_nodes_1000_Nonloveddeldelay), std(kee_nodes_1250_Nonloveddeldelay)]%, std(kee_nodes_1500_Nonloveddeldelay), std(kee_nodes_1750_Nonloveddeldelay)];
kee_nonloved_delay_CI = kee_nonloved_delay_y + kee_nonloved_delay_s.*t.*sqrn

ep_nonloved_delay_x = [250, 500, 750, 1000, 1250]
ep_nonloved_delay_y = [mean(ep_nodes_250_Nonloveddeldelay), mean(ep_nodes_500_Nonloveddeldelay), mean(ep_nodes_750_Nonloveddeldelay), mean(ep_nodes_1000_Nonloveddeldelay), mean(ep_nodes_1250_Nonloveddeldelay)];
ep_nonloved_delay_s = [std(ep_nodes_250_Nonloveddeldelay), std(ep_nodes_500_Nonloveddeldelay), std(ep_nodes_750_Nonloveddeldelay), std(ep_nodes_1000_Nonloveddeldelay), std(ep_nodes_1250_Nonloveddeldelay)];
ep_nonloved_delay_CI = ep_nonloved_delay_y + ep_nonloved_delay_s.*t.*sqrn 

figure(5)
eb=errorbar(kee_nonloved_delay_x, kee_nonloved_delay_y, kee_nonloved_delay_y - kee_nonloved_delay_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_nonloved_delay_x, ep_nonloved_delay_y, ep_nonloved_delay_y - ep_nonloved_delay_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([500 500], [0 216000], '-.k')
hold off
ylim([0 216000])
set(gca,'FontSize',12)
set(gca, 'YTick', [0 36000 72000 108000 144000 180000 216000])
set(gca, 'YTickLabel', {'0h','10h','20h','30h','40h','50h','60h'})
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize', 14)
%title('Non-Popular Delivery delay, with 99% confidence intervals')
xlabel('Number of nodes')
ylabel('Delivery delay [hours]')
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/non_popular_delay_nodes','png')

% overhead

kee_overhead_x = [250, 500, 750, 1000, 1250]%, 1500, 1750]
kee_overhead_y = [mean(kee_nodes_250_Totalbytessent), mean(kee_nodes_500_Totalbytessent), mean(kee_nodes_750_Totalbytessent), mean(kee_nodes_1000_Totalbytessent), mean(kee_nodes_1250_Totalbytessent)]%, mean(kee_nodes_1500_Totalbytessent), mean(kee_nodes_1750_Totalbytessent)];
kee_overhead_s = [std(kee_nodes_250_Totalbytessent), std(kee_nodes_500_Totalbytessent), std(kee_nodes_750_Totalbytessent), std(kee_nodes_1000_Totalbytessent), std(kee_nodes_1250_Totalbytessent)]%, std(kee_nodes_1500_Totalbytessent), std(kee_nodes_1750_Totalbytessent)];
kee_overhead_CI = kee_overhead_y + kee_overhead_s.*t.*sqrn

ep_overhead_x = [250, 500, 750, 1000, 1250]
ep_overhead_y = [mean(ep_nodes_250_Totalbytessent), mean(ep_nodes_500_Totalbytessent), mean(ep_nodes_750_Totalbytessent), mean(ep_nodes_1000_Totalbytessent), mean(ep_nodes_1250_Totalbytessent)]; % to Peta-Bytes directly
ep_overhead_s = [std(ep_nodes_250_Totalbytessent), std(ep_nodes_500_Totalbytessent), std(ep_nodes_750_Totalbytessent), std(ep_nodes_1000_Totalbytessent), std(ep_nodes_1250_Totalbytessent)];
ep_overhead_CI = ep_overhead_y + ep_overhead_s.*t.*sqrn

figure(6)
eb=errorbar(kee_overhead_x, kee_overhead_y, kee_overhead_y - kee_overhead_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_overhead_x, ep_overhead_y, ep_overhead_y - ep_overhead_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([500 500], [0 550000000000], '-.k')
hold off
ylim([0 550000000000])
set(gca, 'YTick', [0 100000000000 200000000000 300000000000 400000000000 500000000000 600000000000])
set(gca, 'YTickLabel', {'1000','2000','3000','4000','5000','6000'})
set(gca,'FontSize',12)
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize', 14)
%title('Overhead in Bytes, with 99% confidence intervals')
xlabel('Number of nodes')
ylabel('Overhead (Yottabytes, 10^8 Bytes)')
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/overhead_nodes','png')

