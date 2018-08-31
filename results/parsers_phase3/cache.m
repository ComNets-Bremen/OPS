% Create cache graphs and statistics
% Tested with Matlab R2016a and Octave 4.4.0
clear all;

% Check if we are in Octave and include required libraries
isOctave = exist('OCTAVE_VERSION', 'builtin') ~= 0;
if (isOctave)
  %% Packages io and signal are required for octave: octave-io octave-signal
  pkg load io;
  pkg load signal;
end


ep__cache_5MB;
ep__cache_3MB;
ep__cache_1MB;
ep__cache_500KB;
ep__cache_100KB;
ep__cache_50KB;
ep__cache_40KB;
ep__cache_20KB;

kee__cache_5MB;
kee__cache_3MB;
kee__cache_1MB;
kee__cache_500KB;
kee__cache_100KB;
kee__cache_50KB;
kee__cache_40KB;
kee__cache_20KB;


t = 2.576;
n = 30;
sqrn = square(n);
format = [0 0 6 3];

% loved del ratio

kee_loved_ratio_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
kee_loved_ratio_y = [mean(kee_5MB_Loveddelratio), mean(kee_3MB_Loveddelratio), mean(kee_1MB_Loveddelratio), mean(kee_500KB_Loveddelratio), mean(kee_100KB_Loveddelratio), mean(kee_50KB_Loveddelratio), mean(kee_40KB_Loveddelratio), mean(kee_20KB_Loveddelratio)]
kee_loved_ratio_s = [std(kee_5MB_Loveddelratio), std(kee_3MB_Loveddelratio), std(kee_1MB_Loveddelratio), std(kee_500KB_Loveddelratio), std(kee_100KB_Loveddelratio), std(kee_50KB_Loveddelratio), std(kee_40KB_Loveddelratio), std(kee_20KB_Loveddelratio)]
kee_loved_ratio_CI = kee_loved_ratio_y + kee_loved_ratio_s.*t.*sqrn

ep_loved_ratio_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
ep_loved_ratio_y = [mean(ep_5MB_Loveddelratio), mean(ep_3MB_Loveddelratio), mean(ep_1MB_Loveddelratio), mean(ep_500KB_Loveddelratio), mean(ep_100KB_Loveddelratio), mean(ep_50KB_Loveddelratio), mean(ep_40KB_Loveddelratio), mean(ep_20KB_Loveddelratio)]
ep_loved_ratio_s = [std(ep_5MB_Loveddelratio), std(ep_3MB_Loveddelratio), std(ep_1MB_Loveddelratio), std(ep_500KB_Loveddelratio), std(ep_100KB_Loveddelratio), std(ep_50KB_Loveddelratio), std(ep_40KB_Loveddelratio), std(ep_20KB_Loveddelratio)]
ep_loved_ratio_CI = ep_loved_ratio_y + ep_loved_ratio_s.*t.*sqrn

figure(1)
eb=errorbar(kee_loved_ratio_x, kee_loved_ratio_y, kee_loved_ratio_y-kee_loved_ratio_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_loved_ratio_x, ep_loved_ratio_y, ep_loved_ratio_y - ep_loved_ratio_CI, '-ob')
set(eb , 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([5000 5000], [0 1], '-.k')
hold off
ylim([0 1])
set(gca,'FontSize',12)
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize',14)
%title('Popular Delivery ratio, with 99% confidence intervals')
xlabel('Cache size per node (Bytes), log scale')
ylabel('Delivery ratio (%)')
set(gca, 'XDir','reverse')
set(gca, 'XScale', 'log')
set(gca, 'XTick', [20, 100, 1000, 5000])
set(gca, 'XTickLabel', {'20KB','100KB','1MB','5MB'})
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/popular_ratio_cache','png')

% non-loved del ratio

kee_non_loved_ratio_x = [5000, 3000, 1000, 500,100, 50, 40, 20];
kee_non_loved_ratio_y = [mean(kee_5MB_Nonloveddelratio), mean(kee_3MB_Nonloveddelratio), mean(kee_1MB_Nonloveddelratio), mean(kee_500KB_Nonloveddelratio), mean(kee_100KB_Nonloveddelratio), mean(kee_50KB_Nonloveddelratio), mean(kee_40KB_Nonloveddelratio), mean(kee_20KB_Nonloveddelratio)]
kee_non_loved_ratio_s = [std(kee_5MB_Nonloveddelratio), std(kee_3MB_Nonloveddelratio), std(kee_1MB_Nonloveddelratio), std(kee_500KB_Nonloveddelratio), std(kee_100KB_Nonloveddelratio), std(kee_50KB_Nonloveddelratio), std(kee_40KB_Nonloveddelratio), std(kee_20KB_Nonloveddelratio)]
kee_non_loved_ratio_CI = kee_non_loved_ratio_y + kee_non_loved_ratio_s.*t.*sqrn

ep_non_loved_ratio_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
ep_non_loved_ratio_y = [mean(ep_5MB_Nonloveddelratio), mean(ep_3MB_Nonloveddelratio), mean(ep_1MB_Nonloveddelratio), mean(ep_500KB_Nonloveddelratio), mean(ep_100KB_Nonloveddelratio), mean(ep_50KB_Nonloveddelratio), mean(ep_40KB_Nonloveddelratio), mean(ep_20KB_Nonloveddelratio)]
ep_non_loved_ratio_s = [std(ep_5MB_Nonloveddelratio), std(ep_3MB_Nonloveddelratio), std(ep_1MB_Nonloveddelratio), std(ep_500KB_Nonloveddelratio), std(ep_100KB_Nonloveddelratio), std(ep_50KB_Nonloveddelratio), std(ep_40KB_Nonloveddelratio), std(ep_20KB_Nonloveddelratio)]
ep_non_loved_ratio_CI = ep_non_loved_ratio_y + ep_non_loved_ratio_s.*t.*sqrn

figure(2)
eb=errorbar(kee_non_loved_ratio_x, kee_non_loved_ratio_y, kee_non_loved_ratio_y - kee_non_loved_ratio_CI , '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_non_loved_ratio_x, ep_non_loved_ratio_y, ep_non_loved_ratio_y - ep_non_loved_ratio_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([5000 5000], [0 1], '-.k')
hold off
ylim([0 1])
set(gca,'FontSize',12)
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize',14)
xlabel('Cache size per node (Bytes), log scale')
ylabel('Delivery ratio (%)')
set(gca, 'XDir','reverse')
set(gca, 'XScale', 'log')
set(gca, 'XTick', [20, 100, 1000, 5000])
set(gca, 'XTickLabel', {'20KB','100KB','1MB','5MB'})
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition',  format);
saveas(gcf,'../matlab_graphs/non_popular_ratio_cache','png')

% delivery delay

kee_loved_delay_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
kee_loved_delay_y = [mean(kee_5MB_Loveddeldelay), mean(kee_3MB_Loveddeldelay), mean(kee_1MB_Loveddeldelay), mean(kee_500KB_Loveddeldelay), mean(kee_100KB_Loveddeldelay), mean(kee_50KB_Loveddeldelay), mean(kee_40KB_Loveddeldelay), mean(kee_20KB_Loveddeldelay)]
kee_loved_delay_s = [std(kee_5MB_Loveddeldelay), std(kee_3MB_Loveddeldelay), std(kee_1MB_Loveddeldelay), std(kee_500KB_Loveddeldelay), std(kee_100KB_Loveddeldelay), std(kee_50KB_Loveddeldelay), std(kee_40KB_Loveddeldelay), std(kee_20KB_Loveddeldelay)]
kee_loved_delay_CI = kee_loved_delay_y + kee_loved_delay_s.*t.*sqrn

ep_loved_delay_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
ep_loved_delay_y = [mean(ep_5MB_Loveddeldelay), mean(ep_3MB_Loveddeldelay), mean(ep_1MB_Loveddeldelay), mean(ep_500KB_Loveddeldelay), mean(ep_100KB_Loveddeldelay), mean(ep_50KB_Loveddeldelay), mean(ep_40KB_Loveddeldelay), mean(ep_20KB_Loveddeldelay)]
ep_loved_delay_s = [std(ep_5MB_Loveddeldelay), std(ep_3MB_Loveddeldelay), std(ep_1MB_Loveddeldelay), std(ep_500KB_Loveddeldelay), std(ep_100KB_Loveddeldelay), std(ep_50KB_Loveddeldelay), std(ep_40KB_Loveddeldelay), std(ep_20KB_Loveddeldelay)]
ep_loved_delay_CI = ep_loved_delay_y + ep_loved_delay_s.*t.*sqrn

figure(3)
eb=errorbar(kee_loved_delay_x, kee_loved_delay_y, kee_loved_delay_y - kee_loved_delay_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_loved_delay_x, ep_loved_delay_y, ep_loved_delay_y - ep_loved_delay_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([5000 5000], [0 21600], '-.k')
hold off
ylim([0 21600])
set(gca, 'YTick', [0 3600 7200 10800 14400 18000 21600])
set(gca, 'YTickLabel', {'0h','1h','2h','3h','4h','5h','6h'})
set(gca,'FontSize',12)
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize',14)
%title('Loved Delivery delay, with 99% confidence intervals')
xlabel('Cache size per node (Bytes), log scale')
ylabel('Delivery delay [hours]')
set(gca, 'XDir','reverse')
set(gca, 'XScale', 'log')
set(gca, 'XTick', [20, 100, 1000, 5000])
set(gca, 'XTickLabel', {'20KB','100KB','1MB','5MB'})
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/popular_delay_cache','png')

% non loved delivery delay

kee_non_loved_delay_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
kee_non_loved_delay_y = [mean(kee_5MB_Nonloveddeldelay), mean(kee_3MB_Nonloveddeldelay), mean(kee_1MB_Nonloveddeldelay), mean(kee_500KB_Nonloveddeldelay), mean(kee_100KB_Nonloveddeldelay), mean(kee_50KB_Nonloveddeldelay), mean(kee_40KB_Nonloveddeldelay), mean(kee_20KB_Nonloveddeldelay)]
kee_non_loved_delay_s = [std(kee_5MB_Nonloveddeldelay), std(kee_3MB_Nonloveddeldelay), std(kee_1MB_Nonloveddeldelay), std(kee_500KB_Nonloveddeldelay), std(kee_100KB_Nonloveddeldelay), std(kee_50KB_Nonloveddeldelay), std(kee_40KB_Nonloveddeldelay), std(kee_20KB_Nonloveddeldelay)]
kee_non_loved_delay_CI = kee_non_loved_delay_y + kee_non_loved_delay_s.*t.*sqrn

ep_non_loved_delay_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
ep_non_loved_delay_y = [mean(ep_5MB_Nonloveddeldelay), mean(ep_3MB_Nonloveddeldelay), mean(ep_1MB_Nonloveddeldelay), mean(ep_500KB_Nonloveddeldelay), mean(ep_100KB_Nonloveddeldelay), mean(ep_50KB_Nonloveddeldelay), mean(ep_40KB_Nonloveddeldelay), mean(ep_20KB_Nonloveddeldelay)]
ep_non_loved_delay_s = [std(ep_5MB_Nonloveddeldelay), std(ep_3MB_Nonloveddeldelay), std(ep_1MB_Nonloveddeldelay), std(ep_500KB_Nonloveddeldelay), std(ep_100KB_Nonloveddeldelay), std(ep_50KB_Nonloveddeldelay), std(ep_40KB_Nonloveddeldelay), std(ep_20KB_Nonloveddeldelay)]
ep_non_loved_delay_CI = ep_non_loved_delay_y + ep_non_loved_delay_s.*t.*sqrn

figure(4)
eb=errorbar(kee_non_loved_delay_x, kee_non_loved_delay_y, kee_non_loved_delay_y - kee_non_loved_delay_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_non_loved_delay_x, ep_non_loved_delay_y, ep_non_loved_delay_y - ep_non_loved_delay_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([5000 5000], [0 216000], '-.k')
hold off
set(gca,'FontSize',12)
ylim([0 216000])
set(gca, 'YTick', [0 36000 72000 108000 144000 180000 216000])
set(gca, 'YTickLabel', {'0h','10h','20h','30h','40h','50h','60h'})
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize',14)
%title('Non-Loved Delivery delay, with 99% confidence intervals')
xlabel('Cache size per node (Bytes), log scale')
ylabel('Delivery delay [hours]')
set(gca, 'XDir','reverse')
set(gca, 'XScale', 'log')
set(gca, 'XTick', [20, 100, 1000, 5000])
set(gca, 'XTickLabel', {'20KB','100KB','1MB','5MB'})
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/non_popular_delay_cache','png')

% overhead

kee_overhead_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
kee_overhead_y = [mean(kee_5MB_Totalbytessent), mean(kee_3MB_Totalbytessent), mean(kee_1MB_Totalbytessent), mean(kee_500KB_Totalbytessent), mean(kee_100KB_Totalbytessent), mean(kee_50KB_Totalbytessent), mean(kee_40KB_Totalbytessent), mean(kee_20KB_Totalbytessent)]
kee_overhead_s = [std(kee_5MB_Totalbytessent), std(kee_3MB_Totalbytessent), std(kee_1MB_Totalbytessent), std(kee_500KB_Totalbytessent), std(kee_100KB_Totalbytessent), std(kee_50KB_Totalbytessent), std(kee_40KB_Totalbytessent), std(kee_20KB_Totalbytessent)]
kee_overhead_CI = kee_overhead_y + kee_overhead_s.*t.*sqrn

ep_overhead_x = [5000, 3000, 1000, 500, 100, 50, 40, 20];
ep_overhead_y = [mean(ep_5MB_Totalbytessent), mean(ep_3MB_Totalbytessent), mean(ep_1MB_Totalbytessent), mean(ep_500KB_Totalbytessent), mean(ep_100KB_Totalbytessent), mean(ep_50KB_Totalbytessent), mean(ep_40KB_Totalbytessent), mean(ep_20KB_Totalbytessent)]
ep_overhead_s = [std(ep_5MB_Totalbytessent), std(ep_3MB_Totalbytessent), std(ep_1MB_Totalbytessent), std(ep_500KB_Totalbytessent), std(ep_100KB_Totalbytessent), std(ep_50KB_Totalbytessent), std(ep_40KB_Totalbytessent), std(ep_20KB_Totalbytessent)]
ep_overhead_CI = ep_overhead_y + ep_overhead_s.*t.*sqrn

figure(5)
eb=errorbar(kee_overhead_x, kee_overhead_y, kee_overhead_y - kee_overhead_CI, '-xr')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
hold on;
eb=errorbar(ep_overhead_x, ep_overhead_y, ep_overhead_y - ep_overhead_CI, '-ob')
set(eb, 'LineWidth', 1)
set(eb, 'MarkerSize', 10)
plot([5000 5000], [0 550000000000], '-.k')
hold off
set(gca,'FontSize',12)
ylim([0 550000000000])
set(gca, 'YTick', [0 100000000000 200000000000 300000000000 400000000000 500000000000 600000000000])
set(gca, 'YTickLabel', {'1000','2000','3000','4000','5000','6000'})
ylabel('Overhead (Yottabytes, 10^8 Bytes)')
%legend({'Keetchi', 'Epidemic', 'Reference scenario'}, 'FontSize',14)
%title('Overhead in Bytes, with 99% confidence intervals')
xlabel('Cache size per node (Bytes), log scale')
set(gca, 'XDir','reverse')
set(gca, 'XScale', 'log')
set(gca, 'XTick', [20, 100, 1000, 5000])
set(gca, 'XTickLabel', {'20KB','100KB','1MB','5MB'})
fig = gcf;
set(fig, 'PaperUnits', 'inches');
set(fig, 'PaperPosition', format);
saveas(gcf,'../matlab_graphs/overhead_cache','png')

