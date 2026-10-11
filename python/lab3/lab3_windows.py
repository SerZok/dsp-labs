import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import warnings

warnings.filterwarnings("ignore", message="This window is not suitable for spectral analysis")

# Настройки
N = 50          # Порядок фильтра
M = N + 1       # Длина окна
fc = 0.25       # Нормированная частота среза (0..1, где 1 = Fs/2)
nfft = 8192     # Разрешение БПФ для точного измерения лепестков

windows_list = [
    ("Прямоугольное", "boxcar", {}),
    ("Треугольное", "triang", {}),
    ("Бартлетта", "bartlett", {}),
    ("Хана", "hann", {}),
    ("Хемминга", "hamming", {}),
    ("Блекмена", "blackman", {}),
    ("Кайзера β=4", "kaiser", {"beta": 4}),
    ("Кайзера β=9", "kaiser", {"beta": 9}),
    ("Чебышева β=40дБ", "chebwin", {"at": 40}),
    ("Чебышева β=90дБ", "chebwin", {"at": 90}),
    ("Бартлетта-Ханна", "barthann", {}),
    ("Блекмена-Харриса", "blackmanharris", {}),
    ("Бомена", "bohman", {}),
    ("Гаусса", "gaussian", {"std": 0.4 * M}),
    ("Модиф. Блекмена-Харриса", "nuttall", {}),
    ("Тьюки", "tukey", {"alpha": 0.5}),
]

print(f"{'№':<3} | {'Окно':<25} | {'Шир. гл. лепестка':<20} | {'1-й бок. лепесток (дБ)':<22} | {'Затухание ПН (дБ)'}")
print("-" * 95)

results = []

for idx, (name, win_type, kwargs) in enumerate(windows_list, 1):
    if kwargs:
        window_arg = (win_type, tuple(kwargs.values())[0])
    else:
        window_arg = win_type
    
    # 1. Генерируем окно
    win = signal.get_window(window_arg, M)
    
    # 2. Измеряем спектр окна
    W = np.fft.fft(win, nfft)
    W_mag = np.abs(W[:nfft//2])
    W_db = 20 * np.log10(W_mag / np.max(W_mag) + 1e-12)
    freqs = np.linspace(0, 1, nfft//2) # Нормированная частота 0..1 (где 1 = Fs/2)
    
    first_null_idx = -1
    for i in range(2, len(W_db) - 1):
        if W_db[i] < W_db[i-1] and W_db[i] < W_db[i+1] and W_db[i] < -15:
            first_null_idx = i
            break
            
    # Если вдруг не нашли (для экзотических случаев), берем fallback
    if first_null_idx == -1:
        first_null_idx = np.argmin(W_db[1:200]) + 1
        
    # Ширина главного лепестка = расстояние между двумя первыми нулями (симметрично)
    main_lobe_width = 2 * freqs[first_null_idx]
    
    # Уровень первого бокового лепестка (первый локальный максимум ПОСЛЕ первого нуля)
    side_lobe_region = W_db[first_null_idx + 5 : first_null_idx + 500]
    first_side_lobe_db = np.max(side_lobe_region) if len(side_lobe_region) > 0 else -999
    
    # 3. Проектируем тестовый ФНЧ с этим окном
    h = signal.firwin(N + 1, fc, window=window_arg)
    
    # 4. Измеряем затухание в полосе непропускания
    w_freq, H = signal.freqz(h, worN=nfft)
    H_db = 20 * np.log10(np.abs(H) + 1e-12)
    norm_freq = w_freq / np.pi
    
    # Полоса непропускания начинается с fc + 0.1 (с запасом на переходную полосу)
    stopband_mask = norm_freq >= (fc + 0.1)
    min_stopband_atten = -np.max(H_db[stopband_mask])
    
    results.append((idx, name, main_lobe_width, first_side_lobe_db, min_stopband_atten))
    
    print(f"{idx:<3} | {name:<25} | {main_lobe_width:<20.4f} | {first_side_lobe_db:<22.1f} | {min_stopband_atten:.1f}")

print("-" * 95)

# ==========================================
# ПОСТРОЕНИЕ ГРАФИКОВ
# ==========================================
plt.rcParams['font.size'] = 9

# --- График 1: Формы окон ---
fig1, axes1 = plt.subplots(4, 4, figsize=(14, 10))
fig1.suptitle("Рис. 1. Формы оконных функций (M = 51)", fontsize=12, fontweight='bold')
for idx, (name, win_type, kwargs) in enumerate(windows_list):
    window_arg = (win_type, tuple(kwargs.values())[0]) if kwargs else win_type
    ax = axes1[idx // 4, idx % 4]
    win = signal.get_window(window_arg, M)
    ax.plot(win, 'b-', linewidth=1)
    ax.set_title(name, fontsize=8)
    ax.set_ylim(-0.05, 1.05)
    ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("fig1_window_shapes.png", dpi=150)
plt.show()

# --- График 2: Спектры окон ---
fig2, axes2 = plt.subplots(4, 4, figsize=(14, 10))
fig2.suptitle("Рис. 2. Спектры оконных функций (дБ)", fontsize=12, fontweight='bold')
for idx, (name, win_type, kwargs) in enumerate(windows_list):
    window_arg = (win_type, tuple(kwargs.values())[0]) if kwargs else win_type
    ax = axes2[idx // 4, idx % 4]
    win = signal.get_window(window_arg, M)
    W = np.fft.fft(win, 4096)
    W_db = 20 * np.log10(np.abs(W[:2048]) / np.max(np.abs(W[:2048])) + 1e-12)
    freqs = np.linspace(0, 0.5, 2048)
    ax.plot(freqs, W_db, 'b-', linewidth=0.8)
    ax.set_title(name, fontsize=8)
    ax.set_ylim(-120, 5)
    ax.axhline(y=-60, color='r', linestyle='--', alpha=0.5, linewidth=0.8)
    ax.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig("fig2_window_spectra.png", dpi=150)
plt.show()

# --- График 3: АЧХ тестовых фильтров ---
fig3, ax3 = plt.subplots(1, 1, figsize=(12, 6))
fig3.suptitle("Рис. 3. АЧХ тестового ФНЧ (N=50, fc=0.25) с различными окнами", fontsize=12, fontweight='bold')
colors = plt.cm.tab20(np.linspace(0, 1, len(windows_list)))

for idx, (name, win_type, kwargs) in enumerate(windows_list):
    window_arg = (win_type, tuple(kwargs.values())[0]) if kwargs else win_type
    
    h = signal.firwin(N + 1, fc, window=window_arg)
    w, H = signal.freqz(h, worN=2048)
    H_db = 20 * np.log10(np.abs(H) + 1e-12)
    norm_freq = w / np.pi
    ax3.plot(norm_freq, H_db, linewidth=1.2, color=colors[idx], label=name)

ax3.set_xlabel("Нормированная частота (доля от Fs/2)")
ax3.set_ylabel("АЧХ (дБ)")
ax3.set_xlim(0, 1)
ax3.set_ylim(-120, 5)
ax3.axhline(y=-60, color='r', linestyle='--', alpha=0.7, label='Требование: -60 дБ')
ax3.axvline(x=fc, color='g', linestyle=':', alpha=0.7, label=f'fc = {fc}')
ax3.grid(True, alpha=0.3)

ax3.legend(fontsize=8, loc='lower left', ncol=2)
plt.tight_layout()
plt.savefig("fig3_test_filter_afc.png", dpi=150)
plt.show()