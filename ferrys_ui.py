"""
╔══════════════════════════════════════════════════════════╗
║   FERRYS MARGARITA — Interfaz de Simulación Visual       ║
║   Universidad de Carabobo · Fundamentos de Programación  ║
╚══════════════════════════════════════════════════════════╝
Requiere: Python 3.8+, customtkinter, matplotlib
  pip install customtkinter matplotlib
"""

import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import re
import os
import threading
import time

try:
    import customtkinter as ctk
    CTK = True
    ctk.set_appearance_mode("dark")
    ctk.set_default_color_theme("blue")
except ImportError:
    CTK = False

try:
    import matplotlib
    matplotlib.use("TkAgg")
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
    MPL = True
except ImportError:
    MPL = False

# ─── PALETA ──────────────────────────────────────────────────────────────────
C = {
    "bg":        "#0d1117",
    "panel":     "#161b22",
    "card":      "#1c2333",
    "border":    "#30363d",
    "accent":    "#58a6ff",
    "accent2":   "#3fb950",
    "warn":      "#f78166",
    "gold":      "#e3b341",
    "text":      "#e6edf3",
    "muted":     "#8b949e",
    "express":   "#388bfd",
    "trad":      "#3fb950",
    "emerg":     "#f85149",
    "carga_st":  "#58a6ff",
    "viaje_st":  "#3fb950",
    "espera_st": "#8b949e",
    # vehicle type colors
    "liviano":   "#58a6ff",
    "rustico":   "#3fb950",
    "microbus":  "#e3b341",
    "carga":     "#a371f7",
    "ambulancia":"#f85149",
    "bomberos":  "#f78166",
    "policial":  "#ff7b72",
}

TIPO_COLOR = {
    "liviano":   C["liviano"],
    "rustico":   C["rustico"],
    "microbus":  C["microbus"],
    "carga":     C["carga"],
    "ambulancia":C["ambulancia"],
    "bomberos":  C["bomberos"],
    "policial":  C["policial"],
    "desconocido": C["muted"],
}

TIPO_EMOJI = {
    "liviano":    "🚗",
    "rustico":    "🚙",
    "microbus":   "🚐",
    "carga":      "🚛",
    "ambulancia": "🚑",
    "bomberos":   "🚒",
    "policial":   "🚔",
    "desconocido":"🚘",
}

FERRY_COLOR = {
    "Lilia Concepcion": C["express"],
    "La Isabela":       C["trad"],
    "La Margaritena":   "#a371f7",
}

# ─── PARSER ──────────────────────────────────────────────────────────────────
def parse_out(filepath):
    """Parse proy1.out and return (viajes, stats) dicts."""
    viajes = []
    stats  = {}
    current = None

    with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
        lines = f.readlines()

    in_stats = False
    for raw in lines:
        line = raw.strip()
        if not line:
            continue

        # ── Inicio de carga ──
        m = re.match(r"Carga Nro\.\s*(\d+)\s*:", line)
        if m:
            if current:
                viajes.append(current)
            current = {"numero": int(m.group(1)), "vehiculos": [], "ferry": "",
                       "num_vehiculos": 0, "num_pasajeros": 0, "tercera_edad": 0,
                       "peso": 0.0, "ingreso": 0.0}
            in_stats = False
            continue

        if current is None and not in_stats:
            continue

        # ── Campos del viaje ──
        if current:
            m = re.match(r"Ferry\s+(.+)", line)
            if m:
                current["ferry"] = m.group(1).strip()
                continue
            m = re.match(r"Num\.\s+vehiculos:\s*(\d+)", line)
            if m:
                current["num_vehiculos"] = int(m.group(1))
                continue
            m = re.match(r"Num\.\s+pasajeros:\s*(\d+)\s*\((\d+)", line)
            if m:
                current["num_pasajeros"] = int(m.group(1))
                current["tercera_edad"]  = int(m.group(2))
                continue
            m = re.match(r"Peso:\s*([\d.]+)", line)
            if m:
                current["peso"] = float(m.group(1))
                continue
            m = re.match(r"Ingreso:\s*([\d.]+)", line)
            if m:
                current["ingreso"] = float(m.group(1))
                continue
            m = re.match(r"ID:\s*(\S+)\s+Tipo:\s*(\S+)", line)
            if m:
                current["vehiculos"].append({"placa": m.group(1), "tipo": m.group(2)})
                continue

        # ── Estadísticas ──
        if re.match(r"Estadisticas", line):
            if current:
                viajes.append(current)
                current = None
            in_stats = True
            continue

        if in_stats:
            m = re.match(r"Total vehiculos transportados:\s*(\d+)", line)
            if m: stats["total_vehiculos"] = int(m.group(1)); continue
            m = re.match(r"Total pasajeros transportados:\s*(\d+)", line)
            if m: stats["total_pasajeros"] = int(m.group(1)); continue
            m = re.match(r"Total de ingresos:\s*([\d.]+)", line)
            if m: stats["total_ingresos"] = float(m.group(1)); continue
            m = re.match(r"Num\.\s+pasajeros no trasladados:\s*(\d+)", line)
            if m: stats["varados"] = int(m.group(1)); continue
            m = re.match(r"Vehiculo mas frecuente:\s*(\S+)", line)
            if m: stats["mas_frecuente"] = m.group(1); continue
            m = re.match(r"Mayor frec\. vehiculos en espera:\s*(\d+)\s*\(a las\s*(\S+)\)", line)
            if m:
                stats["mayor_frec"]  = int(m.group(1))
                stats["hora_pico"]   = m.group(2)
                continue

    if current:
        viajes.append(current)

    return viajes, stats

# ─── HELPERS UI ──────────────────────────────────────────────────────────────
def make_frame(parent, **kw):
    defaults = {"bg": C["panel"]}
    defaults.update(kw)
    return tk.Frame(parent, **defaults)

def make_label(parent, text="", size=11, bold=False, color=None, **kw):
    font = ("Consolas" if not bold else "Consolas", size, "bold" if bold else "normal")
    cfg  = {"bg": kw.pop("bg", C["panel"]), "fg": color or C["text"],
            "font": font, "text": text}
    cfg.update(kw)
    return tk.Label(parent, **cfg)

def make_btn(parent, text, cmd, color=None, **kw):
    bg = color or C["accent"]
    return tk.Button(parent, text=text, command=cmd,
                     bg=bg, fg=C["bg"], font=("Consolas", 10, "bold"),
                     relief="flat", bd=0, padx=12, pady=6,
                     activebackground=C["muted"], cursor="hand2", **kw)

def card(parent, title="", width=None, height=None):
    outer = tk.Frame(parent, bg=C["border"], padx=1, pady=1)
    inner = tk.Frame(outer, bg=C["card"])
    inner.pack(fill="both", expand=True)
    if title:
        tk.Label(inner, text=title, bg=C["card"], fg=C["accent"],
                 font=("Consolas", 10, "bold"), pady=6).pack(anchor="w", padx=10)
        tk.Frame(inner, bg=C["border"], height=1).pack(fill="x")
    return outer, inner

def vehicle_badge(parent, tipo, placa=""):
    col  = TIPO_COLOR.get(tipo, C["muted"])
    emj  = TIPO_EMOJI.get(tipo, "🚘")
    is_e = tipo in ("ambulancia", "bomberos", "policial")
    bg   = C["card"]
    f = tk.Frame(parent, bg=bg, padx=4, pady=2)
    tk.Label(f, text=emj, bg=bg, fg=col, font=("Consolas", 12)).pack(side="left")
    if placa:
        tk.Label(f, text=placa, bg=bg, fg=col if is_e else C["text"],
                 font=("Consolas", 9, "bold" if is_e else "normal")).pack(side="left", padx=2)
    return f

# ─── APP ─────────────────────────────────────────────────────────────────────
class FerrysApp:
    def __init__(self, root):
        self.root  = root
        self.viajes = []
        self.stats  = {}
        self.sim_running  = False
        self.sim_index    = 0
        self.sim_speed    = 1.0
        self._sim_thread  = None

        self._build_root()
        self._build_tabs()
        self._show_empty()

    # ── ROOT WINDOW ──────────────────────────────────────────────────────────
    def _build_root(self):
        self.root.title("⚓  Ferrys Margarita — Simulación Visual")
        self.root.configure(bg=C["bg"])
        self.root.geometry("1200x780")
        self.root.minsize(900, 600)

        # Top bar
        bar = tk.Frame(self.root, bg=C["panel"], height=56)
        bar.pack(fill="x", side="top")
        bar.pack_propagate(False)

        tk.Label(bar, text="⚓  FERRYS MARGARITA", bg=C["panel"], fg=C["accent"],
                 font=("Consolas", 16, "bold")).pack(side="left", padx=20, pady=10)
        tk.Label(bar, text="Sistema de Simulación de Transporte Marítimo",
                 bg=C["panel"], fg=C["muted"], font=("Consolas", 10)).pack(side="left", pady=10)

        make_btn(bar, "📂  Cargar proy1.out", self._load_file,
                 color=C["accent"]).pack(side="right", padx=16, pady=10)

        tk.Frame(self.root, bg=C["border"], height=1).pack(fill="x")

    # ── TABS ─────────────────────────────────────────────────────────────────
    def _build_tabs(self):
        style = ttk.Style()
        style.theme_use("default")
        style.configure("Dark.TNotebook",
                        background=C["bg"], borderwidth=0)
        style.configure("Dark.TNotebook.Tab",
                        background=C["panel"], foreground=C["muted"],
                        font=("Consolas", 11, "bold"),
                        padding=[20, 8], borderwidth=0)
        style.map("Dark.TNotebook.Tab",
                  background=[("selected", C["bg"])],
                  foreground=[("selected", C["accent"])])

        self.nb = ttk.Notebook(self.root, style="Dark.TNotebook")
        self.nb.pack(fill="both", expand=True, padx=0, pady=0)

        self.tab_sim   = tk.Frame(self.nb, bg=C["bg"])
        self.tab_rep   = tk.Frame(self.nb, bg=C["bg"])
        self.tab_stats = tk.Frame(self.nb, bg=C["bg"])

        self.nb.add(self.tab_sim,   text="  🚢  Simulación  ")
        self.nb.add(self.tab_rep,   text="  📋  Reportes  ")
        self.nb.add(self.tab_stats, text="  📊  Estadísticas  ")

    # ── EMPTY STATE ──────────────────────────────────────────────────────────
    def _show_empty(self):
        for tab in (self.tab_sim, self.tab_rep, self.tab_stats):
            for w in tab.winfo_children():
                w.destroy()
            f = tk.Frame(tab, bg=C["bg"])
            f.place(relx=0.5, rely=0.5, anchor="center")
            tk.Label(f, text="⚓", bg=C["bg"], fg=C["border"],
                     font=("Consolas", 64)).pack()
            tk.Label(f, text="Carga el archivo  proy1.out  para comenzar",
                     bg=C["bg"], fg=C["muted"],
                     font=("Consolas", 13)).pack(pady=8)
            make_btn(f, "📂  Abrir archivo", self._load_file,
                     color=C["accent"]).pack(pady=6)

    # ── FILE LOAD ─────────────────────────────────────────────────────────────
    def _load_file(self):
        path = filedialog.askopenfilename(
            title="Seleccionar proy1.out",
            filetypes=[("Archivos de salida", "*.out"), ("Todos", "*.*")])
        if not path:
            return
        try:
            self.viajes, self.stats = parse_out(path)
            if not self.viajes:
                messagebox.showwarning("Advertencia",
                    "No se encontraron viajes en el archivo.\n"
                    "Verifica que sea un proy1.out válido.")
                return
            self._build_sim_tab()
            self._build_rep_tab()
            self._build_stats_tab()
            messagebox.showinfo("✅  Cargado",
                f"Se cargaron {len(self.viajes)} viaje(s) correctamente.")
        except Exception as e:
            messagebox.showerror("Error al parsear", str(e))

    # ══════════════════════════════════════════════════════════════════════════
    # TAB 1 — SIMULACIÓN
    # ══════════════════════════════════════════════════════════════════════════
    def _build_sim_tab(self):
        for w in self.tab_sim.winfo_children():
            w.destroy()
        self.sim_index   = 0
        self.sim_running = False

        # ── Controls bar ──
        ctrl = tk.Frame(self.tab_sim, bg=C["panel"], height=52)
        ctrl.pack(fill="x", padx=0, pady=0)
        ctrl.pack_propagate(False)

        self.lbl_clock = tk.Label(ctrl, text="🕐  --:--", bg=C["panel"],
                                  fg=C["gold"], font=("Consolas", 14, "bold"))
        self.lbl_clock.pack(side="left", padx=18)

        tk.Frame(ctrl, bg=C["border"], width=1).pack(side="left", fill="y", pady=8)

        self.btn_play = make_btn(ctrl, "▶  Play",  self._sim_play,  color=C["accent2"])
        self.btn_play.pack(side="left", padx=8, pady=8)
        make_btn(ctrl, "⏸  Pause", self._sim_pause, color=C["gold"]).pack(side="left", padx=4, pady=8)
        make_btn(ctrl, "⏮  Reset", self._sim_reset, color=C["warn"]).pack(side="left", padx=4, pady=8)

        tk.Frame(ctrl, bg=C["border"], width=1).pack(side="left", fill="y", pady=8, padx=8)
        tk.Label(ctrl, text="Velocidad:", bg=C["panel"], fg=C["muted"],
                 font=("Consolas", 9)).pack(side="left")
        self.speed_var = tk.DoubleVar(value=1.0)
        spd = tk.Scale(ctrl, from_=0.5, to=3.0, resolution=0.5,
                       orient="horizontal", variable=self.speed_var,
                       bg=C["panel"], fg=C["text"], troughcolor=C["border"],
                       highlightthickness=0, length=100, showvalue=True,
                       font=("Consolas", 8))
        spd.pack(side="left", padx=4)

        self.lbl_viaje_num = tk.Label(ctrl, text="Viaje 0 / 0",
                                      bg=C["panel"], fg=C["muted"],
                                      font=("Consolas", 10))
        self.lbl_viaje_num.pack(side="right", padx=18)

        tk.Frame(self.tab_sim, bg=C["border"], height=1).pack(fill="x")

        # ── Main area ──
        main = tk.Frame(self.tab_sim, bg=C["bg"])
        main.pack(fill="both", expand=True, padx=12, pady=10)

        # Left: ferry status
        left = tk.Frame(main, bg=C["bg"], width=340)
        left.pack(side="left", fill="y", padx=(0, 8))
        left.pack_propagate(False)

        tk.Label(left, text="FERRYS EN EL MUELLE", bg=C["bg"], fg=C["accent"],
                 font=("Consolas", 10, "bold")).pack(anchor="w", pady=(0, 6))

        self.ferry_frames = {}
        ferrys_info = [
            ("Lilia Concepcion", "EXPRESS",     16, 50),
            ("La Isabela",       "TRADICIONAL", 20, 70),
            ("La Margaritena",   "TRADICIONAL", 18, 60),
        ]
        for fname, ftype, vcap, pcap in ferrys_info:
            col_f = FERRY_COLOR.get(fname, C["accent"])
            o, i = card(left)
            o.pack(fill="x", pady=3)
            i.configure(padx=10, pady=6)

            top = tk.Frame(i, bg=C["card"])
            top.pack(fill="x")
            tk.Label(top, text=f"🚢 {fname}", bg=C["card"], fg=col_f,
                     font=("Consolas", 10, "bold")).pack(side="left")
            lbl_st = tk.Label(top, text="● ESPERA", bg=C["card"], fg=C["espera_st"],
                              font=("Consolas", 9, "bold"))
            lbl_st.pack(side="right")

            info_row = tk.Frame(i, bg=C["card"])
            info_row.pack(fill="x", pady=2)
            tk.Label(info_row, text=f"{ftype}", bg=C["card"], fg=C["muted"],
                     font=("Consolas", 8)).pack(side="left")
            lbl_veh = tk.Label(info_row, text="Vehículos: 0", bg=C["card"],
                               fg=C["text"], font=("Consolas", 9))
            lbl_veh.pack(side="right")

            # mini vehicle strip
            strip = tk.Frame(i, bg=C["card"], height=28)
            strip.pack(fill="x", pady=(2, 0))

            self.ferry_frames[fname] = {
                "lbl_estado": lbl_st,
                "lbl_veh":    lbl_veh,
                "strip":      strip,
                "vcap":       vcap,
            }

        # Right: vehicle queues
        right = tk.Frame(main, bg=C["bg"])
        right.pack(side="left", fill="both", expand=True)

        tk.Label(right, text="COLAS DE VEHÍCULOS", bg=C["bg"], fg=C["accent"],
                 font=("Consolas", 10, "bold")).pack(anchor="w", pady=(0, 6))

        # Express queue
        oe, ie = card(right, "🔵  Cola Express")
        oe.pack(fill="x", pady=3)
        self.cola_express_frame = tk.Frame(ie, bg=C["card"], height=60)
        self.cola_express_frame.pack(fill="x", padx=8, pady=6)

        # Tradicional queue
        ot, it_ = card(right, "🟢  Cola Tradicional")
        ot.pack(fill="x", pady=3)
        self.cola_trad_frame = tk.Frame(it_, bg=C["card"], height=60)
        self.cola_trad_frame.pack(fill="x", padx=8, pady=6)

        # Current trip info
        oci, ici = card(right, "📦  Vehículos en este viaje")
        oci.pack(fill="both", expand=True, pady=3)
        self.viaje_scroll_frame = tk.Frame(ici, bg=C["card"])
        self.viaje_scroll_frame.pack(fill="both", expand=True, padx=8, pady=6)

        self._sim_render()

    def _sim_render(self):
        """Render current simulation frame."""
        idx = self.sim_index
        total = len(self.viajes)
        self.lbl_viaje_num.config(text=f"Viaje  {min(idx+1, total)}  /  {total}")

        if idx >= total:
            self.lbl_clock.config(text="🕐  FIN")
            return

        v = self.viajes[idx]

        # Clock: estimate time from viaje numero (approximate)
        base_min = 480 + idx * 35
        hh = (base_min // 60) % 24
        mm = base_min % 60
        self.lbl_clock.config(text=f"🕐  {hh:02d}:{mm:02d}")

        # Ferry states
        fname = v["ferry"]
        for fn, fd in self.ferry_frames.items():
            if fn == fname:
                estado_txt = "● CARGANDO"
                estado_col = C["carga_st"]
            else:
                estado_txt = "● ESPERA"
                estado_col = C["espera_st"]
            fd["lbl_estado"].config(text=estado_txt, fg=estado_col)
            fd["lbl_veh"].config(text=f"Vehículos: {v['num_vehiculos'] if fn == fname else 0}")

            # mini strip
            strip = fd["strip"]
            for w in strip.winfo_children():
                w.destroy()
            if fn == fname:
                for veh in v["vehiculos"][:16]:
                    col_v = TIPO_COLOR.get(veh["tipo"], C["muted"])
                    tk.Label(strip, text="■", bg=C["card"], fg=col_v,
                             font=("Consolas", 14)).pack(side="left", padx=1)

        # Queues visualization (show vehicles of next trips as "waiting")
        for fr in (self.cola_express_frame, self.cola_trad_frame):
            for w in fr.winfo_children():
                w.destroy()

        future_vehs = []
        for j in range(idx + 1, min(idx + 4, total)):
            future_vehs.extend(self.viajes[j]["vehiculos"][:4])

        exp_vehs  = future_vehs[:6]
        trad_vehs = future_vehs[6:12]

        for veh in exp_vehs:
            vehicle_badge(self.cola_express_frame, veh["tipo"], veh["placa"][:7]).pack(side="left", padx=2, pady=2)
        if not exp_vehs:
            tk.Label(self.cola_express_frame, text="Cola vacía", bg=C["card"],
                     fg=C["muted"], font=("Consolas", 9)).pack(padx=8)

        for veh in trad_vehs:
            vehicle_badge(self.cola_trad_frame, veh["tipo"], veh["placa"][:7]).pack(side="left", padx=2, pady=2)
        if not trad_vehs:
            tk.Label(self.cola_trad_frame, text="Cola vacía", bg=C["card"],
                     fg=C["muted"], font=("Consolas", 9)).pack(padx=8)

        # Current trip vehicles
        for w in self.viaje_scroll_frame.winfo_children():
            w.destroy()
        row = None
        for ci, veh in enumerate(v["vehiculos"]):
            if ci % 4 == 0:
                row = tk.Frame(self.viaje_scroll_frame, bg=C["card"])
                row.pack(fill="x", pady=1)
            vehicle_badge(row, veh["tipo"], veh["placa"]).pack(side="left", padx=4, pady=2)

    def _sim_play(self):
        if self.sim_running:
            return
        self.sim_running = True
        self._sim_thread = threading.Thread(target=self._sim_loop, daemon=True)
        self._sim_thread.start()

    def _sim_pause(self):
        self.sim_running = False

    def _sim_reset(self):
        self.sim_running = False
        time.sleep(0.15)
        self.sim_index = 0
        self.root.after(0, self._sim_render)

    def _sim_loop(self):
        while self.sim_running and self.sim_index < len(self.viajes):
            self.root.after(0, self._sim_render)
            spd = self.speed_var.get()
            time.sleep(max(0.3, 1.5 / spd))
            self.sim_index += 1
        self.sim_running = False
        self.root.after(0, self._sim_render)

    # ══════════════════════════════════════════════════════════════════════════
    # TAB 2 — REPORTES
    # ══════════════════════════════════════════════════════════════════════════
    def _build_rep_tab(self):
        for w in self.tab_rep.winfo_children():
            w.destroy()

        pane = tk.PanedWindow(self.tab_rep, orient="horizontal",
                              bg=C["bg"], sashwidth=4, sashrelief="flat",
                              sashpad=2)
        pane.pack(fill="both", expand=True, padx=12, pady=10)

        # Left list
        left = tk.Frame(pane, bg=C["bg"], width=260)
        pane.add(left, minsize=200)

        tk.Label(left, text="VIAJES DEL DÍA", bg=C["bg"], fg=C["accent"],
                 font=("Consolas", 10, "bold")).pack(anchor="w", pady=(0, 6))

        list_frame = tk.Frame(left, bg=C["panel"])
        list_frame.pack(fill="both", expand=True)

        sb = tk.Scrollbar(list_frame, bg=C["border"], troughcolor=C["bg"])
        sb.pack(side="right", fill="y")

        self.trip_list = tk.Listbox(list_frame, bg=C["panel"], fg=C["text"],
                                    font=("Consolas", 10),
                                    selectbackground=C["accent"],
                                    selectforeground=C["bg"],
                                    bd=0, highlightthickness=0,
                                    yscrollcommand=sb.set)
        self.trip_list.pack(fill="both", expand=True)
        sb.config(command=self.trip_list.yview)

        for v in self.viajes:
            col = FERRY_COLOR.get(v["ferry"], C["muted"])
            self.trip_list.insert("end", f"  Viaje #{v['numero']:02d}  {v['ferry']}")
        self.trip_list.bind("<<ListboxSelect>>", self._on_trip_select)

        # Right detail
        right = tk.Frame(pane, bg=C["bg"])
        pane.add(right, minsize=400)

        self.detail_frame = tk.Frame(right, bg=C["bg"])
        self.detail_frame.pack(fill="both", expand=True)
        self._show_trip_placeholder()

    def _show_trip_placeholder(self):
        for w in self.detail_frame.winfo_children():
            w.destroy()
        tk.Label(self.detail_frame, text="← Selecciona un viaje",
                 bg=C["bg"], fg=C["muted"], font=("Consolas", 12)).pack(expand=True)

    def _on_trip_select(self, event):
        sel = self.trip_list.curselection()
        if not sel:
            return
        v = self.viajes[sel[0]]
        self._render_trip_detail(v)

    def _render_trip_detail(self, v):
        for w in self.detail_frame.winfo_children():
            w.destroy()

        col_f = FERRY_COLOR.get(v["ferry"], C["accent"])

        # Header
        hdr = tk.Frame(self.detail_frame, bg=C["card"], padx=16, pady=12)
        hdr.pack(fill="x", pady=(0, 6))
        tk.Label(hdr, text=f"🚢  {v['ferry']}", bg=C["card"], fg=col_f,
                 font=("Consolas", 14, "bold")).pack(anchor="w")
        tk.Label(hdr, text=f"Viaje Nro. {v['numero']}",
                 bg=C["card"], fg=C["muted"], font=("Consolas", 10)).pack(anchor="w")

        # Stats cards row
        stats_row = tk.Frame(self.detail_frame, bg=C["bg"])
        stats_row.pack(fill="x", pady=4)

        def mini_card(parent, icon, label, val, color):
            f = tk.Frame(parent, bg=C["card"], padx=12, pady=8)
            f.pack(side="left", fill="both", expand=True, padx=3)
            tk.Label(f, text=icon, bg=C["card"], fg=color,
                     font=("Consolas", 18)).pack()
            tk.Label(f, text=str(val), bg=C["card"], fg=color,
                     font=("Consolas", 13, "bold")).pack()
            tk.Label(f, text=label, bg=C["card"], fg=C["muted"],
                     font=("Consolas", 8)).pack()

        mini_card(stats_row, "🚗", "Vehículos",  v["num_vehiculos"], C["accent"])
        mini_card(stats_row, "👥", "Pasajeros",  v["num_pasajeros"], C["accent2"])
        mini_card(stats_row, "🧓", "3ra Edad",   v["tercera_edad"],  C["gold"])
        mini_card(stats_row, "⚖️", "Toneladas",  f"{v['peso']:.2f}", C["warn"])
        mini_card(stats_row, "💰", "BsF.",        f"{v['ingreso']:.0f}", C["accent2"])

        # Vehicle list
        tk.Label(self.detail_frame, text="VEHÍCULOS TRANSPORTADOS",
                 bg=C["bg"], fg=C["accent"], font=("Consolas", 9, "bold")).pack(
                 anchor="w", padx=4, pady=(8, 4))

        vframe = tk.Frame(self.detail_frame, bg=C["panel"])
        vframe.pack(fill="both", expand=True, padx=0)

        vsb = tk.Scrollbar(vframe, bg=C["border"])
        vsb.pack(side="right", fill="y")

        canvas = tk.Canvas(vframe, bg=C["panel"], bd=0,
                           highlightthickness=0, yscrollcommand=vsb.set)
        canvas.pack(side="left", fill="both", expand=True)
        vsb.config(command=canvas.yview)

        inner = tk.Frame(canvas, bg=C["panel"])
        canvas.create_window((0, 0), window=inner, anchor="nw")

        # header row
        hrow = tk.Frame(inner, bg=C["border"])
        hrow.pack(fill="x", pady=1)
        for txt, w_px in [("  #", 30), ("Placa", 100), ("Tipo", 100), ("Emergencia", 90)]:
            tk.Label(hrow, text=txt, bg=C["border"], fg=C["muted"],
                     font=("Consolas", 9, "bold"), width=w_px//8).pack(side="left", padx=4)

        for ci, veh in enumerate(v["vehiculos"]):
            is_e = veh["tipo"] in ("ambulancia", "bomberos", "policial")
            bg_r = C["card"] if ci % 2 == 0 else C["panel"]
            row  = tk.Frame(inner, bg=bg_r)
            row.pack(fill="x")

            col_v = TIPO_COLOR.get(veh["tipo"], C["muted"])
            emj   = TIPO_EMOJI.get(veh["tipo"], "🚘")
            tk.Label(row, text=f"  {ci+1:02d}", bg=bg_r, fg=C["muted"],
                     font=("Consolas", 9), width=4).pack(side="left", padx=4, pady=3)
            tk.Label(row, text=veh["placa"], bg=bg_r, fg=C["text"],
                     font=("Consolas", 10, "bold"), width=10).pack(side="left", padx=4)
            tk.Label(row, text=f"{emj} {veh['tipo']}", bg=bg_r, fg=col_v,
                     font=("Consolas", 9), width=14).pack(side="left", padx=4)
            if is_e:
                tk.Label(row, text="⚠ PRIORIDAD", bg=bg_r, fg=C["emerg"],
                         font=("Consolas", 8, "bold")).pack(side="left", padx=4)

        inner.update_idletasks()
        canvas.config(scrollregion=canvas.bbox("all"))

    # ══════════════════════════════════════════════════════════════════════════
    # TAB 3 — ESTADÍSTICAS
    # ══════════════════════════════════════════════════════════════════════════
    def _build_stats_tab(self):
        for w in self.tab_stats.winfo_children():
            w.destroy()

        if not self.stats:
            tk.Label(self.tab_stats, text="Sin estadísticas disponibles",
                     bg=C["bg"], fg=C["muted"], font=("Consolas", 12)).pack(expand=True)
            return

        s = self.stats

        # Scrollbars vertical y horizontal
        vsb = tk.Scrollbar(self.tab_stats, orient="vertical")
        hsb = tk.Scrollbar(self.tab_stats, orient="horizontal")
        scroll_canvas = tk.Canvas(self.tab_stats, bg=C["bg"], bd=0, highlightthickness=0,
                                  yscrollcommand=vsb.set, xscrollcommand=hsb.set)
        vsb.config(command=scroll_canvas.yview)
        hsb.config(command=scroll_canvas.xview)

        hsb.pack(side="bottom", fill="x")
        vsb.pack(side="right",  fill="y")
        scroll_canvas.pack(side="left", fill="both", expand=True)

        inner = tk.Frame(scroll_canvas, bg=C["bg"])
        win_id = scroll_canvas.create_window((0, 0), window=inner, anchor="nw")

        def _on_inner_resize(event):
            scroll_canvas.configure(scrollregion=scroll_canvas.bbox("all"))
        inner.bind("<Configure>", _on_inner_resize)

        # Scroll con rueda del mouse
        def _on_mousewheel(event):
            scroll_canvas.yview_scroll(int(-1*(event.delta/120)), "units")
        scroll_canvas.bind_all("<MouseWheel>", _on_mousewheel)
        scroll_canvas.bind_all("<Button-4>", lambda e: scroll_canvas.yview_scroll(-1, "units"))
        scroll_canvas.bind_all("<Button-5>", lambda e: scroll_canvas.yview_scroll( 1, "units"))

        pad = dict(padx=14, pady=8)

        # ── KPI cards row ──
        tk.Label(inner, text="RESUMEN DEL DÍA", bg=C["bg"], fg=C["accent"],
                 font=("Consolas", 11, "bold")).pack(anchor="w", **pad)

        kpi_row = tk.Frame(inner, bg=C["bg"])
        kpi_row.pack(fill="x", padx=14, pady=4)

        kpis = [
            ("🚗", "Vehículos\ntransportados", s.get("total_vehiculos", 0), C["accent"]),
            ("👥", "Pasajeros\ntransportados", s.get("total_pasajeros", 0), C["accent2"]),
            ("💰", "Ingresos\ntotales (BsF.)", f"{s.get('total_ingresos',0):.2f}", C["gold"]),
            ("🚫", "Pasajeros\nno trasladados", s.get("varados", 0), C["warn"]),
            ("🏆", "Vehículo\nmás frecuente", s.get("mas_frecuente","—"), C["accent2"]),
            ("⏰", "Hora pico\nde espera", s.get("hora_pico","—"), C["gold"]),
        ]
        for icon, lbl, val, col in kpis:
            f = tk.Frame(kpi_row, bg=C["card"], padx=14, pady=12,
                         relief="flat", bd=0)
            f.pack(side="left", fill="both", expand=True, padx=4)
            tk.Frame(f, bg=col, height=3).pack(fill="x")
            tk.Label(f, text=icon, bg=C["card"], fg=col,
                     font=("Consolas", 22)).pack(pady=(6, 0))
            tk.Label(f, text=str(val), bg=C["card"], fg=col,
                     font=("Consolas", 14, "bold")).pack()
            tk.Label(f, text=lbl, bg=C["card"], fg=C["muted"],
                     font=("Consolas", 8)).pack(pady=(2, 6))

        if not MPL:
            tk.Label(inner, text="(matplotlib no disponible — instala con: pip install matplotlib)",
                     bg=C["bg"], fg=C["warn"], font=("Consolas", 9)).pack(pady=10)
            return

        # ── Charts row ──
        tk.Label(inner, text="GRÁFICAS", bg=C["bg"], fg=C["accent"],
                 font=("Consolas", 11, "bold")).pack(anchor="w", **pad)

        charts_row = tk.Frame(inner, bg=C["bg"])
        charts_row.pack(fill="x", padx=14, pady=4)

        # Count vehicle types
        tipo_count = {}
        for v in self.viajes:
            for veh in v["vehiculos"]:
                t = veh["tipo"]
                tipo_count[t] = tipo_count.get(t, 0) + 1

        # --- Chart 1: Vehicle types bar chart ---
        fig1, ax1 = plt.subplots(figsize=(5, 3.2))
        fig1.patch.set_facecolor(C["card"])
        ax1.set_facecolor(C["card"])

        tipos  = list(tipo_count.keys())
        counts = list(tipo_count.values())
        bar_colors = [TIPO_COLOR.get(t, C["muted"]) for t in tipos]

        bars = ax1.bar(tipos, counts, color=bar_colors, edgecolor=C["bg"], linewidth=1.5)
        ax1.set_title("Tipos de Vehículos Transportados",
                      color=C["text"], fontsize=9, fontweight="bold", pad=8)
        ax1.tick_params(colors=C["muted"], labelsize=7)
        ax1.set_xlabel("Tipo", color=C["muted"], fontsize=8)
        ax1.set_ylabel("Cantidad", color=C["muted"], fontsize=8)
        for spine in ax1.spines.values():
            spine.set_color(C["border"])
        ax1.xaxis.label.set_color(C["muted"])
        for bar, val in zip(bars, counts):
            ax1.text(bar.get_x() + bar.get_width()/2., bar.get_height() + 0.1,
                     str(val), ha='center', va='bottom',
                     color=C["text"], fontsize=8, fontweight="bold")
        plt.xticks(rotation=30, ha="right")
        plt.tight_layout()

        c1_frame = tk.Frame(charts_row, bg=C["card"], padx=2, pady=2)
        c1_frame.pack(side="left", fill="both", expand=True, padx=4)
        FigureCanvasTkAgg(fig1, master=c1_frame).get_tk_widget().pack(fill="both", expand=True)
        plt.close(fig1)

        # --- Chart 2: Pie chart ferry distribution ---
        ferry_count = {}
        for v in self.viajes:
            fn = v["ferry"]
            ferry_count[fn] = ferry_count.get(fn, 0) + v["num_vehiculos"]

        fig2, ax2 = plt.subplots(figsize=(4.5, 3.2))
        fig2.patch.set_facecolor(C["card"])
        ax2.set_facecolor(C["card"])

        flabels = list(ferry_count.keys())
        fvals   = list(ferry_count.values())
        fcolors = [FERRY_COLOR.get(fl, C["muted"]) for fl in flabels]

        wedges, texts, autotexts = ax2.pie(
            fvals, labels=flabels, colors=fcolors,
            autopct="%1.0f%%", startangle=90,
            textprops={"color": C["text"], "fontsize": 7},
            wedgeprops={"edgecolor": C["bg"], "linewidth": 2})
        for at in autotexts:
            at.set_color(C["bg"])
            at.set_fontweight("bold")
        ax2.set_title("Vehículos por Ferry",
                      color=C["text"], fontsize=9, fontweight="bold", pad=8)
        plt.tight_layout()

        c2_frame = tk.Frame(charts_row, bg=C["card"], padx=2, pady=2)
        c2_frame.pack(side="left", fill="both", expand=True, padx=4)
        FigureCanvasTkAgg(fig2, master=c2_frame).get_tk_widget().pack(fill="both", expand=True)
        plt.close(fig2)

        # --- Chart 3: Ingresos por viaje line chart ---
        fig3, ax3 = plt.subplots(figsize=(5, 3.2))
        fig3.patch.set_facecolor(C["card"])
        ax3.set_facecolor(C["card"])

        nums     = [v["numero"]  for v in self.viajes]
        ingresos = [v["ingreso"] for v in self.viajes]

        ax3.plot(nums, ingresos, color=C["gold"], linewidth=2, marker="o",
                 markersize=5, markerfacecolor=C["accent"], markeredgecolor=C["bg"])
        ax3.fill_between(nums, ingresos, alpha=0.15, color=C["gold"])
        ax3.set_title("Ingresos por Viaje (BsF.)",
                      color=C["text"], fontsize=9, fontweight="bold", pad=8)
        ax3.tick_params(colors=C["muted"], labelsize=7)
        ax3.set_xlabel("Viaje #", color=C["muted"], fontsize=8)
        ax3.set_ylabel("BsF.", color=C["muted"], fontsize=8)
        for spine in ax3.spines.values():
            spine.set_color(C["border"])
        plt.tight_layout()

        c3_frame = tk.Frame(charts_row, bg=C["card"], padx=2, pady=2)
        c3_frame.pack(side="left", fill="both", expand=True, padx=4)
        FigureCanvasTkAgg(fig3, master=c3_frame).get_tk_widget().pack(fill="both", expand=True)
        plt.close(fig3)

        inner.update_idletasks()
        scroll_canvas.config(scrollregion=scroll_canvas.bbox("all"))
        scroll_canvas.bind("<Configure>",
            lambda e: scroll_canvas.itemconfig(win_id, width=max(e.width, inner.winfo_reqwidth())))


# ─── MAIN ─────────────────────────────────────────────────────────────────────
def main():
    root = tk.Tk()
    root.configure(bg=C["bg"])
    try:
        root.iconbitmap("")
    except:
        pass

    app = FerrysApp(root)

    # If a file is passed as argument, load it automatically
    import sys
    if len(sys.argv) > 1 and os.path.isfile(sys.argv[1]):
        try:
            app.viajes, app.stats = parse_out(sys.argv[1])
            if app.viajes:
                app._build_sim_tab()
                app._build_rep_tab()
                app._build_stats_tab()
        except Exception as e:
            print(f"Error cargando archivo: {e}")

    root.mainloop()

if __name__ == "__main__":
    main()
