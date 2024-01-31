/* eslint-disable react/prop-types */

import './MMTClient.scss'
import React from "react"

class Named {
    static Names = {
        all: "All MIDI",
        mix: "Audio Mixer",
        dmx: "Lights",
        home: "Home",
        mqtt: "Smart House",
        mmkey: "Media Keys"
    };

    static get(target) {
        switch (target) {
            case 0: return Named.Names.home;
            case 254: return Named.Names.mix;
            case 253: return Named.Names.mmkey;
            case 252: return Named.Names.mqtt;
            case 250:
            case 251: return Named.Names.dmx;
            case 255: return Named.Names.all;
            default: return target.toString();
        }
    }
}
class MMKeys {
    static Names = {
        zoom: "ZOOM",
        launch: "LAUNCH SELECTED MEDIA",
        play: "PLAY",
        prev: "PREV TRACK",
        next: "NEXT TRACK",
        stop: "STOP",
        ppause: "PLAY/PAUSE",
        vup: "VOLUME UP",
        vdown: "VOLUME DOWN",
        vmute: "VOLUME MUTE",
        pause: "PAUSE"
    };

    static get(target) {
        switch (target) {
            case 75: return MMKeys.Names.zoom;
            case 76: return MMKeys.Names.launch;
            case 70: return MMKeys.Names.play;
            case 66: return MMKeys.Names.prev;
            case 67: return MMKeys.Names.next;
            case 68: return MMKeys.Names.stop;
            case 69: return MMKeys.Names.ppause;
            case 71: return MMKeys.Names.vup;
            case 72: return MMKeys.Names.vdown;
            case 73: return MMKeys.Names.vmute;
            case 74: return MMKeys.Names.pause;
            default: return target.toString();
        }
    }
}
class MMTColorPicker extends React.Component {

    static borderdefault = 'rgba(91, 91, 91, 1)';
    static colordefault = 'rgba(0,255,243,1)';
    constructor(props) {
        super(props);
        this.state = {
            picker: {
                id: ((props.picker.id === undefined) || (props.picker.id === null)) ? 0 : props.picker.id,
                target: ((props.picker.target === undefined) || (props.picker.target === null)) ? 0 : props.picker.target,
                r: ((props.picker.r === undefined) || (props.picker.r === null)) ? 0 : props.picker.r,
                g: ((props.picker.g === undefined) || (props.picker.g === null)) ? 0 : props.picker.g,
                b: ((props.picker.b === undefined) || (props.picker.b === null)) ? 0 : props.picker.b,
                w: ((props.picker.w === undefined) || (props.picker.w === null)) ? 0 : props.picker.w,
                scene: ((props.picker.scene === undefined) || (props.picker.scene === null)) ? 0 : props.picker.scene,
                lvalue: ((props.picker.lvalue === undefined) || (props.picker.lvalue === null)) ? 0 : props.picker.lvalue,
                cvalue: ((props.picker.cvalue === undefined) || (props.picker.cvalue === null)) ? 127 : props.picker.cvalue,
                wvalue: ((props.picker.wvalue === undefined) || (props.picker.wvalue === null)) ? 0 : props.picker.wvalue,
                color: ((props.picker.color === undefined) || (props.picker.color === null) || (props.picker.color === '')) ? MMTColorPicker.colordefault : props.picker.color
            },
            colorborder: MMTColorPicker.borderdefault,
            colorlight: '',
            colorsmotch: '',
            focus: false
        };
        this.state.colorlight = `linear-gradient(to bottom, black, ${this.state.picker.color})`;
        this.state.colorsmotch = `linear-gradient(to bottom, white, ${this.state.picker.color})`;
        this.label = `R:${this.state.picker.r} G:${this.state.picker.g} B:${this.state.picker.b} ${(this.state.picker.w > 0) ? 'W:' : ''}${(this.state.picker.w > 0) ? this.state.picker.w : ''}`;
        this.colors = [
            { color: [255, 0, 0, 1], position: 0 },
            { color: [255, 0, 255, 1], position: 42 },
            { color: [0, 0, 255, 1], position: 84 },
            { color: [0, 255, 255, 1], position: 126 },
            { color: [0, 255, 0, 1], position: 168 },
            { color: [255, 255, 0, 1], position: 210 },
            { color: [255, 0, 0, 1], position: 255 }
        ];
        this.setRgba = this.setRgba.bind(this);
        this.handleFocus = this.handleFocus.bind(this);
        this.handleRemove = this.handleRemove.bind(this);
        this.handleSubmit = this.handleSubmit.bind(this);
        this.handleWhiteChange = this.handleWhiteChange.bind(this);
        this.handleColorChange = this.handleColorChange.bind(this);
        this.handleLightsChange = this.handleLightsChange.bind(this);
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }

    getClosest_(position) {
        let left = this.colors[0],
            right = { color: [0, 0, 0], position: Number.MAX_VALUE };
        let leftIndex = 0;
        for (let i = 0; i < this.colors.length; i++) {
            if (position >= this.colors[i].position && position > left.position) {
                left = this.colors[i];
                leftIndex = i;
            }
        }
        if (this.colors.length - 1 === leftIndex) {
            right = this.colors[leftIndex];
        }
        else {
            right = this.colors[leftIndex + 1];
        }
        if (left == right) {
            return [right, right, 0];
        }
        let dleft = position - left.position;
        let sum = dleft + right.position - position;
        let weight = dleft / sum;
        return [left, right, weight];
    }
    getRgbaFromRange_(c1, c2, weight) {
        let w1 = weight;
        let w2 = 1 - w1;
        let rgba = [
            Math.round(c1[0] * w2 + c2[0] * w1),
            Math.round(c1[1] * w2 + c2[1] * w1),
            Math.round(c1[2] * w2 + c2[2] * w1),
            1
        ];
        return rgba;
    }
    getRgba_(position) {
        let [l, r, weight] = this.getClosest_(position);
        return this.getRgbaFromRange_(l.color, r.color, weight);
    }
    setRgba() {

        const clz = this;
        const p = new Promise(function () {
            let picker = clz.state.picker;
            const c = clz.getRgba_(picker.cvalue);
            if (picker.wvalue > 0) {

                const v = Math.round(255 * -(picker.wvalue / 255));
                c[0] = Math.max(0, Math.min(255, c[0] - v));
                c[1] = Math.max(0, Math.min(255, c[1] - v));
                c[2] = Math.max(0, Math.min(255, c[2] - v));

                clz.updateControl(picker.w, picker.scene, picker.wvalue);
            }
            if (picker.lvalue > 0) {

                c[0] = Math.max(0, c[0] - Math.floor(picker.lvalue / 100 * 255));
                c[1] = Math.max(0, c[1] - Math.floor(picker.lvalue / 100 * 255));
                c[2] = Math.max(0, c[2] - Math.floor(picker.lvalue / 100 * 255));
            }
            picker.color = `rgba(${c[0]},${c[1]},${c[2]},${c[3]})`;

            clz.setState({
                picker: picker,
                colorborder: picker.color,
                colorlight: `linear-gradient(to bottom, black, ${picker.color})`,
                colorsmotch: `linear-gradient(to bottom, white, ${picker.color})`
            });

            clz.updateControl(picker.r, picker.scene, c[0]);
            clz.updateControl(picker.g, picker.scene, c[1]);
            clz.updateControl(picker.b, picker.scene, c[2]);

            if (clz.props.up !== null)
                clz.props.up(picker);
        });
        p.catch(() => {});
    }

    updateControl(id, scene, val) {
        if (this.props.update !== null)
            this.props.update(
                {
                    action: "change",
                    unit: {
                        id: id,
                        scene: scene,
                        target: this.props.picker.target,
                        value: (val > 1) ? Math.max(0, val / 2) : 0,
                        onoff: false,
                        type: 1
                    }
                }, true
            );
    }

    handleLightsChange(e) {
        let p = this.state.picker;
        if (p.lvalue === e.target.value) return;
        p.lvalue = e.target.value;
        this.setState({ picker: p });
        this.setRgba();
        return false;
    }
    handleColorChange(e) {
        let p = this.state.picker;
        if (p.cvalue === e.target.value) return;
        p.cvalue = e.target.value;
        this.setState({ picker: p });
        this.setRgba();
        return false;
    }
    handleWhiteChange(e) {
        let p = this.state.picker;
        if (p.wvalue === e.target.value) return;
        p.wvalue = e.target.value;
        this.setState({ picker: p });
        this.setRgba();
        return false;
    }
    handleFocus(b) {
        if (this.state.focus === b) return;
        const c = b ? this.state.picker.color : MMTColorPicker.borderdefault;
        this.setState({ focus: b, colorborder: c });
    }
    handleRemove() {
        if ((this.props.rm === undefined) || (this.props.rm === null))
            return;
        this.props.rm(this.state.picker);
    }
    handleSubmit(e) {
        if ((e === undefined) || (e === null))
            return false;
        e.preventDefault();
        e.stopPropagation();
        return false;
    }
    render() {
        if (this.state.picker.target === 0)
            return (
                <React.Fragment>
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                    <div className="color-picker">
                        <div className="color-picker-body" style={{ borderColor: this.state.colorborder }}
                            onPointerEnter={() => this.handleFocus(true)} onPointerLeave={() => this.handleFocus(false)} onFocus={() => this.handleFocus(true)}>
                            <div className="color-picker-whell">
                                <input type="range"
                                    onChange={(e) => this.handleLightsChange(e)}
                                    min="0" max="100" step="0" orient="vertical" defaultValue={this.state.picker.lvalue} />
                                <div className="range-bg-right"
                                    style={{ backgroundImage: this.state.colorlight }}></div>
                                <input type="range"
                                    onChange={(e) => this.handleWhiteChange(e)}
                                    min="0" max="255" step="0" orient="vertical" defaultValue={this.state.picker.wvalue}
                                    style={{ display: (this.state.picker.w === 0) ? 'none' : 'inline-block' }} />
                                <div className="range-bg-right"
                                    style={{ display: (this.state.picker.w === 0) ? 'none' : 'inline-block', backgroundImage: this.state.colorsmotch }}></div>
                                <input type="range"
                                    onChange={(e) => this.handleColorChange(e)}
                                    min="0" max="255" step="0" orient="vertical" defaultValue={this.state.picker.cvalue} />
                                <div className="range-bg-right-color"></div>
                            </div>
                        </div>
                        <div className="labeled-picker">
                            <span className="labeled-span">{this.label}</span>
                            <div className="labeled-content labeled-center" >
                                <ul>
                                    <li className="li-color" onClick={() => this.handleRemove()}>Remove picker?</li>
                                </ul>
                            </div>
                        </div>
                    </div>
                </React.Fragment>
            );
    }
}
class MMTColorPickers extends React.Component {

    static storagetag = 'pickers';
    constructor(props) {
        super(props);
        let json = localStorage.getItem(MMTColorPickers.storagetag);
        let all = ((json !== undefined) && (json !== null) && (json !== '')) ? JSON.parse(json) : [];
        let arr = (Array.isArray(all) && (this.props.target !== undefined) && (this.props.target !== null)) ? all.filter(item => item.target === this.props.target) : [];
        this.state = {
            all: Array.isArray(all) ? all : [],
            pickers: Array.isArray(arr) ? arr : [],
            showsetup: false
        };
        this.handleOpen = this.handleOpen.bind(this);
        this.handleSubmit = this.handleSubmit.bind(this);
        this.handleRemove = this.handleRemove.bind(this);
        this.handleUpdate = this.handleUpdate.bind(this);
        console.log(this.props.target, all, arr);
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    saveStorage(ele, b) {
        let arr = this.state.all;
        if (b) arr.push(ele);
        else arr = arr.filter(item => item.id !== ele.id);
        this.setState({ all: arr });
        localStorage.setItem(MMTColorPickers.storagetag, JSON.stringify(arr));
    }
    handleOpen() {
        this.setState({ showsetup: !this.state.showsetup });
    }
    handleSubmit(e) {
        if ((e === undefined) || (e === null) || (e.target === undefined) || (e.target === null))
            return false;
        e.preventDefault();
        let p = {
            r: ((e.target.R.value === undefined) || (e.target.R.value === null) || (e.target.R.value === '')) ? 0 : parseInt(e.target.R.value),
            g: ((e.target.G.value === undefined) || (e.target.G.value === null) || (e.target.G.value === '')) ? 0 : parseInt(e.target.G.value),
            b: ((e.target.B.value === undefined) || (e.target.B.value === null) || (e.target.B.value === '')) ? 0 : parseInt(e.target.B.value),
            w: ((e.target.W.value === undefined) || (e.target.W.value === null) || (e.target.W.value === '')) ? 0 : parseInt(e.target.W.value),
            scene: ((e.target.S.value === undefined) || (e.target.S.value === null) || (e.target.S.value === '')) ? 0 : parseInt(e.target.S.value),
            target: ((this.props.target === undefined) || (this.props.target === null)) ? 0 : this.props.target,
            id: 0,
            color: '',
            cvalue: 127,
            wvalue: 0,
            lvalue: 0
        };
        if ((p.r !== 0) && (p.g !== 0) && (p.b !== 0) && (p.scene !== 0) &&
            (p.r !== p.g) && (p.r !== p.b) && (p.g !== p.b)) {

            p.id = p.r + p.g + p.b + p.scene;
            let arr = this.state.pickers;
            arr.push(p);
            this.setState({ pickers: arr });
            this.saveStorage(p, true);
        }
        e.stopPropagation();
        this.setState({ showsetup: false });
        return false;
    }
    handleRemove(p) {
        if ((p.id === undefined) || (p.id === null) || (p.id === 0)) return;
        let arr = this.state.pickers.filter(item => item.id !== p.id);
        this.setState({ pickers: arr });
        this.saveStorage(p, false);
    }
    handleUpdate(p) {
        if ((p.id === undefined) || (p.id === null) || (p.id === 0)) return;
        const ele = this.state.all.find((item) => item.id === p.id);
        if ((ele === undefined) || (ele === null)) return;
        ele.color = p.color;
        ele.cvalue = p.cvalue;
        ele.wvalue = p.wvalue;
        ele.lvalue = p.lvalue;
        localStorage.setItem(MMTColorPickers.storagetag, JSON.stringify(this.state.all));
    }
    render() {
        if ((this.props.target !== undefined) && (this.props.target !== null) && ((this.props.target >= 250) && (this.props.target <= 252)))
            return (
                <React.Fragment>
                    <div className={(this.state.pickers.length > 0) ? 'color-picker-form color-picker-next' : 'color-picker-form color-picker-first'} >
                        <div className="color-picker-add" onClick={() => this.handleOpen()}>+</div>
                        <div className="color-picker-setup" style={{ display: (this.state.showsetup) ? 'inline-flex' : 'none' }}>
                            <form name="rgba" key="221222" id="221222" onSubmit={(e) => this.handleSubmit(e)} autoComplete="true">
                                <input type="number" name="R" min="1" max="255" placeholder="RED"></input>
                                <input type="number" name="G" min="1" max="255" placeholder="GREEN"></input>
                                <input type="number" name="B" min="1" max="255" placeholder="BLUE"></input>
                                <input type="number" name="W" min="1" max="255" placeholder="WHITE*"></input>
                                <input type="number" name="S" min="1" max="255" placeholder="SCENE"></input>
                                <input type="submit" name="add" value="Add"></input>
                            </form>
                        </div>
                    </div>
                    {this.state.pickers.map((picker) => {
                        return <MMTColorPicker key={picker.id} picker={picker} update={this.props.update} rm={this.handleRemove} up={this.handleUpdate} />
                    })}
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                </React.Fragment>
            );
    }
}
class MMTdmenu extends React.Component {
    constructor(props) {
        super(props);
        this.url = "https://claudiacoord.github.io/MIDI-MT";
        this.wiki = "https://claudiacoord.github.io/MIDI-MT/docs/EN/";
        this.logClick = this.logClick.bind(this);
        this.wakeupClick = this.wakeupClick.bind(this);
    }
    named(target) {
        return Named.get(target);
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    wakeupClick() {
        if ((this.props.cmd !== undefined) && (this.props.cmd !== null))
            this.props.cmd(
                {
                    action: "wakeup"
                }
            );
        return false;
    }
    logClick() {
        if ((this.props.cmd !== undefined) && (this.props.cmd !== null))
            this.props.cmd(
                {
                    action: "getlog"
                }
            );
        return false;
    }
    render() {
        if ((this.props.fullmenu !== undefined) && (this.props.fullmenu !== null) && (this.props.fullmenu === true))
            return (
                <React.Fragment>
                    <div className="right-mdown">
                        <button className="right-mdown-btn">&nbsp;</button>
                        <div className="right-mdown-content">
                            <a href="#" onClick={() => this.logClick()}>Log</a>
                            <a href="#" onClick={() => this.wakeupClick()}>WakeUp</a>
                            <a href={this.wiki} target="_blank" rel="noreferrer">WIKI&#47;FAQ</a>
                            <a href={this.url} target="_blank" rel="noreferrer">MIDI-MT</a>
                        </div>
                    </div>
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                    <a className="right-menu" href={this.url} target="_blank" rel="noreferrer">MIDI-MT</a>
                </React.Fragment>
            );

    }
}
class MMTnav extends React.Component {
    constructor(props) {
        super(props);
        this.handleClick = this.handleClick.bind(this);
    }
    named(target) {
        return Named.get(target);
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    handleClick() {
        this.props.navigate(this.props.target);
        return false;
    }
    render() {
        return (
            <React.Fragment>
                <a href="#" onClick={(e) => this.handleClick(e)}
                    className={(this.props.actived ? 'active' : 'passive')}
                >{this.named(this.props.target)}</a>
            </React.Fragment>
        );
    }
}
class MMTbctrl extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            id: props.id,
            onoff: props.unit.onoff ? true : false,
            focus: false
        };
        this.handleClick = this.handleClick.bind(this);
        this.handleFocus = this.handleFocus.bind(this);
    }
    static getDerivedStateFromProps(props, state) {
        if ((props.id === state.id) && (props.unit.onoff !== state.onoff)) {
            return {
                onoff: props.unit.onoff
            };
        }
        return null;
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    handleClick() {
        const onoff = !this.state.onoff;

        if (this.props.update !== null)
            this.props.update(
                {
                    action: "change",
                    unit: {
                        id: this.props.unit.id,
                        scene: this.props.unit.scene,
                        target: this.props.unit.target,
                        value: onoff ? 127 : 0,
                        onoff: onoff,
                        type: 3
                    }
                }, true
            );
        return false;
    }
    handleFocus(b) {
        if (this.state.focus === b) return;
        this.setState({ focus: b });
        console.log(b);
    }
    render() {
        if ((this.props.unit.target !== undefined) && (this.props.unit.target !== null) && (this.props.unit.target === 253))
            return (
                <React.Fragment>
                    <input type="button" onClick={() => this.handleClick()} onPointerEnter={() => this.handleFocus(true)} onPointerLeave={() => this.handleFocus(false)} onFocus={() => this.handleFocus(true)}
                        value={`${this.props.unit.id}/${this.props.unit.scene}`}
                        className={(this.state.onoff ? 'on' : 'off')}
                    />
                    <div className="labeled">
                        <div className="labeled-content labeled-mmkeys" style={{ display: (this.state.focus ? 'block' : 'none') }}>
                            <ul>
                                <li>{MMKeys.get(this.props.unit.longtarget)}</li>
                            </ul>
                        </div>
                    </div>
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                    <input type="button" onClick={() => this.handleClick()}
                        value={`${this.props.unit.id}/${this.props.unit.scene}`}
                        className={(this.state.onoff ? 'on' : 'off')}
                    />
                </React.Fragment>
            );
    }
}
class MMTvinfo extends React.Component {
    constructor(props) {
        super(props);
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    render() {
        if (this.props.unit.apps.length > 0)
            return (
                <React.Fragment>
                    <div className="labeled-slider">
                        <span className="labeled-span">{`${this.props.unit.id}/${this.props.unit.scene}`}</span>
                        <div className="labeled-content">
                            <ul>
                                {this.props.unit.apps.map((app, index) => <li key={index}>{app}</li>)}
                            </ul>
                        </div>
                    </div>
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                    <div className="labeled-slider">
                        <span className="labeled-span">{`${this.props.unit.id}/${this.props.unit.scene}`}</span>
                    </div>
                </React.Fragment>
            );

    }
}
class MMTvctrl extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            id: props.id,
            value: props.unit.value
        };
        this.handleChange = this.handleChange.bind(this);
    }
    static getDerivedStateFromProps(props, state) {
        if ((props.id === state.id) && (props.unit.value !== state.value)) {
            return {
                value: props.unit.value
            };
        }
        return null;
    }
    handleChange(e) {
        if (this.state.value === e.target.value) return;

        this.setState({ value: e.target.value });

        if (this.props.update !== null)
            this.props.update(
                    {
                        action: "change",
                        unit: {
                            id: this.props.unit.id,
                            scene: this.props.unit.scene,
                            target: this.props.unit.target,
                            value: (this.props.unit.invert) ? (127 - e.target.value) : e.target.value,
                            onoff: false,
                            type: 1
                        }
                    }, true
            );
        return false;
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    render() {
        return (
            <React.Fragment>
                <div className="slider">
                    <input type="range"
                        onChange={(e) => this.handleChange(e)}
                        min="0" max="127" step="1" orient="vertical" value={this.state.value} list="l1" />
                    <MMTvinfo unit={this.props.unit} />
                </div>
            </React.Fragment>
        );
    }
}
class MMTctrls extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            isfs: false
        };
        this.get_unit_key_ = this.get_unit_key_.bind(this);
        this.get_href_key_ = this.get_href_key_.bind(this);
        this.handleClickFS = this.handleClickFS.bind(this);
        this.handleSubmit = this.handleSubmit.bind(this);
        this.isfs = false;
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    get_unit_key_(u) {
        return (u.scene * 1000000) + (u.id * 1000) + u.longtarget;
    }
    get_href_key_(i) {
        return (this.props.data.target * 1000) + (i * 100);
    }
    submit_group_253_(e) {
        if ((this.props.cmd === undefined) || (this.props.cmd === null))
            return;
        let app = e.target.app.value;
        if (app !== "")
            this.props.cmd(
                {
                    action: "windowtotop",
                    name: app
                }
            );
    }
    handleClickFS() {
        if (!this.state.isfs) {
            const el = document.body;
            const rfs = el.requestFullscreen
                || el.webkitRequestFullScreen
                || el.mozRequestFullScreen
                || el.msRequestFullScreen;
            if (rfs) {
                rfs.call(el);
                this.setState({ isfs: true });
            }

        } else {
            const el = document;
            const rfs = el.exitFullscreen
                || el.webkitExitFullscreen
                || el.mozCancelFullScreen
                || el.msExitFullscreen;
            if (rfs) {
                rfs.call(el);
                this.setState({ isfs: false });
            }
        }
        return false;
    }
    handleSubmit(e) {
        if ((e === undefined) || (e === null))
            return false;
        e.preventDefault();
        this.submit_group_253_(e);
        e.stopPropagation();
        return false;
    }

    render() {
        let isdata = (this.props.data !== undefined) && (this.props.data !== null);
        let isempty = (!isdata || (this.props.data.vctrl === undefined) || (this.props.data.bctrl === undefined) || (this.props.data.nav === undefined) ||
            ((this.props.data.vctrl.length === 0) && (this.props.data.bctrl.length === 0) && (this.props.data.nav.length === 2)));
        let islog = (this.props.log !== undefined) && (this.props.log !== null) && (typeof this.props.log === 'string') && (this.props.log !== '');

        if (isempty)
            return (
                <React.Fragment key="10001">
                    <nav>
                        <div className="loader"></div>
                        <MMTdmenu id="101010" fullmenu={false} />
                        <a className={`right-btn ${(this.state.isfs ? 'icon-ns' : 'icon-fs')}`} href="#" onClick={() => this.handleClickFS()}>&nbsp;</a>
                    </nav>
                    <br />
                    <h3 className="h3-top-level">Waiting...</h3>
                    <div className="awaiting-line">
                        <br />I&apos;m waiting for data to load from the <b>MIDI-MT</b> application.
                        <br /><b>MIDI-MT</b> must be configured and running.
                    </div>
                    <br />
                    <h3 className="h3-next-level h3-warning" style={{ visibility: (this.props.iserror) ? 'visible' : 'hidden' }}>Warning!</h3>
                    <div className="awaiting-error" style={{ visibility: (this.props.iserror) ? 'visible' : 'hidden' }}>
                        <p>{this.props.data.errors.what}</p>
                    </div>
                </React.Fragment>
            );
        else if (this.props.data.target === 0)
            return (
                <React.Fragment key="10002">
                    <nav>
                        {this.props.data.nav.map((target, index) =>
                            <MMTnav target={target} key={this.get_href_key_(index)} actived={(this.props.data.target === target)} navigate={this.props.navigate} />
                        )}
                        <MMTdmenu id="101011" cmd={this.props.cmd} fullmenu={true} />
                        <a className={`right-btn ${(this.state.isfs ? 'icon-ns' : 'icon-fs')}`} href="#" onClick={() => this.handleClickFS()}>&nbsp;</a>
                    </nav>
                    <br />
                    <h3 className="h3-top-level">About</h3>
                    <div className="awaiting-line">
                        <br />You are in the <b>MIDI-MT</b> remote control app.
                        <br />Manage all the controls that are described in the <b>MIDI-MT</b> configuration.
                        <br /><b>MIDI-MT</b> must be configured and running.
                    </div>
                    <div className="block-visable" style={{ display: (this.props.iserror) ? 'block' : 'none' }}>
                        <br />
                        <h3 className="h3-next-level h3-warning">Warning!</h3>
                        <div className="awaiting-error">
                            <p><span>Info:</span>&nbsp;{this.props.data.errors.what}</p>
                        </div>
                    </div>
                    <div className="block-visable" style={{ display: (islog) ? 'block' : 'none' }}>
                        <br />
                        <h3 className="h3-next-level">Log</h3>
                        <div className="awaiting-log">
                            <pre>{this.props.log}</pre>
                        </div>
                    </div>
                </React.Fragment>
            );
        else if (this.props.data.target === 253)
            return (
                <React.Fragment>
                    <nav>
                        {this.props.data.nav.map((target, index) =>
                            <MMTnav target={target} key={this.get_href_key_(index)} actived={(this.props.data.target === target)} navigate={this.props.navigate} />
                        )}
                        <MMTdmenu id="101010" fullmenu={false} />
                        <a className={`right-btn ${(this.state.isfs ? 'icon-ns' : 'icon-fs')}`} href="#" onClick={() => this.handleClickFS()}>&nbsp;</a>
                    </nav>
                    <br/>
                    <h3 className="h3-top-level">{Named.get(this.props.data.target)}</h3>
                    <form id="app-focus" onSubmit={(e) => this.handleSubmit(e)} autoComplete="true">
                        <div className="scrollable-no">
                            <span>
                                You can enter the name of the executable file of the application that you want to bring to the foreground.
                                <br />In some cases, this is necessary to control from media keys.
                                <br />Media button controls do not work on the same machine due to the target application losing focus.<br />
                            </span>
                            <input type="text" name="app" placeholder="vlc.exe, reaper64.exe or obs64, e.t.c."></input>
                            <input type="submit" name="submit" value="Bring application window to Top"/>
                        </div>
                    </form>
                    <div className="scrollable-b" style={{ visibility: (this.props.data.bctrl.length === 0) ? 'hidden' : 'visible' }}>
                        {this.props.data.bctrl.map((unit) => {
                            const k = this.get_unit_key_(unit);
                            return <MMTbctrl key={k} id={k} unit={unit} update={this.props.update} />
                        })}
                    </div>
                    <footer>
                        <div className="footer-left">
                            <p>Configuration: <span>{this.props.data.config.name}</span></p>
                            <p>MIDI-MT: <span>{this.props.data.config.build}</span></p>
                            <p>Status: <span>{(this.props.isconnected ? 'Connected' : 'Disconnected..')} {(this.props.isconnected ? ('to host: ' + location.hostname + ', port: ' + location.port) : '')}</span></p>
                        </div>
                        <div className="footer-right" style={{ visibility: (this.props.iserror) ? 'visible' : 'hidden' }}>
                            <p><span>Info:</span>&nbsp;{this.props.data.errors.what}</p>
                        </div>
                    </footer>
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                    <nav>
                        {this.props.data.nav.map((target, index) =>
                            <MMTnav target={target} key={this.get_href_key_(index)} actived={(this.props.data.target === target)} navigate={this.props.navigate} />
                        )}
                        <MMTdmenu id="101010" fullmenu={false} />
                        <a className={`right-btn ${(this.state.isfs ? 'icon-ns' : 'icon-fs')}`} href="#" onClick={() => this.handleClickFS()}>&nbsp;</a>
                    </nav>
                    <br />
                    <h3  className="h3-top-level">{Named.get(this.props.data.target)}</h3>
                    <div className={((this.props.data.target >= 250) && (this.props.data.target <= 252)) ? 'scrollable-v-picker' : 'scrollable-v-normal'}
                         style={{ visibility: (this.props.data.vctrl.length === 0) ? 'hidden' : 'visible' }}>
                        <MMTColorPickers key={(this.props.data.target + 121212)} component={"div"} update={this.props.update} target={this.props.data.target} />
                        {this.props.data.vctrl.map((unit) => {
                            const k = this.get_unit_key_(unit);
                            return <MMTvctrl key={k} id={k} unit={unit} update={this.props.update} />
                        })}
                        <datalist id="l1">
                            <option value="0" label="0"></option>
                            <option value="64" label="50"></option>
                            <option value="127" label="100"></option>
                        </datalist>
                    </div>
                    <div className="scrollable-b" style={{ visibility: (this.props.data.bctrl.length === 0) ? 'hidden' : 'visible' }}>
                        {this.props.data.bctrl.map((unit) => {
                            const k = this.get_unit_key_(unit);
                            return <MMTbctrl key={k} id={k} unit={unit} update={this.props.update} />
                        })}
                    </div>
                    <footer>
                        <div className="footer-left">
                            <p>Configuration: <span>{this.props.data.config.name}</span></p>
                            <p>MIDI-MT: <span>{this.props.data.config.build}</span></p>
                            <p>Status: <span>{(this.props.isconnected ? 'Connected' : 'Disconnected..')} {(this.props.isconnected ? ('to host: ' + location.hostname + ', port: ' + location.port) : '')}</span></p>
                        </div>
                        <div className="footer-right" style={{ visibility: (this.props.iserror) ? 'visible' : 'hidden' }}>
                            <p><span>Info:</span>&nbsp;{this.props.data.errors.what}</p>
                        </div>
                    </footer>
                </React.Fragment>
            );
    }
}
class MMTClient extends React.Component {

    static storagetag = 'target';
    constructor(props) {
        super(props);
        this.state = {
            data: this.data_default(0, '', '', ''),
            log: '',
            isembed: ((props.isembed !== undefined) && (props.isembed !== null)) ? props.isembed : false
        };
        this.ws = null;
        this.wsonce = false;
        this.source = [];
        this.update = this.update.bind(this);
        this.command = this.command.bind(this);
        this.parse = this.parse.bind(this);
        this.parse_find = this.parse_find.bind(this);
        this.parse_insert = this.parse_insert.bind(this);
        this.navigate = this.navigate.bind(this);
        this.is_error = this.is_error.bind(this);
        this.is_websocket = this.is_websocket.bind(this);
        this.error_default = this.error_default.bind(this);
        this.catch_default = this.catch_default.bind(this);
        this.WS_URL = this.build_ws_url();
    }

    async componentDidMount() {
        await this.connect();
    }
    async componentDidUnMount() {
        if (this.ws !== null) {
            await this.ws.close();
            this.ws = null;
        }
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }

    connect = () => {
        const clz = this;
        let ws = new WebSocket(clz.WS_URL);
        let wstimeout = 2500;
        let connectInterval = null;
        clz.wsonce = false;

        ws.onopen = () => {

            if (connectInterval !== null)
                clearTimeout(connectInterval);

            clz.ws = ws;
            wstimeout = 2500;

            if (!clz.wsonce) {
                clz.wsonce = true;
                ws.send(JSON.stringify({ action: "config" }));
            }
        };
        ws.onclose = err => {
            wstimeout = wstimeout + wstimeout;
            const tm = Math.min(10000, wstimeout);
            connectInterval = setTimeout(clz.check, tm);
            clz.setState({ data: this.data_default(0, "", "", `Socket is closed ${err.reason}. Reconnect will be attempted in ${Math.min(10, (tm / 1000))} second.`) });
        };
        ws.onerror = err => {
            ws.close();
            clz.ws = null;
            clz.setState({ data: this.data_default(0, "", "", `Socket encountered error: ${err.message}, closing socket..`) });
        };
        ws.onmessage = evt => {
            try {
                const d = JSON.parse(evt.data);
                if ((d !== undefined) && (d !== null) && (d.action !== undefined) && (d.action !== null)) {

                    if ((d.action === "config") && (d.units !== undefined) && (d.units !== null) && Array.isArray(d.units)) {

                        const p = new Promise(function () {
                            clz.source = d.units;
                            let target = parseInt(localStorage.getItem(MMTClient.storagetag));
                            clz.parse(target, d.config.name, d.config.build);
                        });
                        p.catch((e) => {
                            clz.catch_default(e);
                        });

                    } else if ((d.action === "changed") && (this.source !== undefined) && (this.source !== null) && Array.isArray(this.source)) {

                        clz.update(d, false);
                        return;

                    } else if ((d.action === "getlog") && (d.log !== undefined) && (d.log !== null) && (typeof d.log === 'string')) {

                        clz.setState({ log: new TextDecoder().decode(Uint8Array.from(window.atob(d.log), (m) => m.codePointAt(0))) });
                        return;
                    }

                } else {
                    clz.setState({ data: clz.data_default(0, "", "", "") });
                }

            } catch (e) {
                this.catch_default(e);
            }
        };
    };

    catch_default(err) {
        if (!(err instanceof Error))
            err = new Error(err);
        this.error_default(err.message);
    }

    error_default(msg) {
        const data = this.state.data;
        data.errors.what = msg;
        this.setState({ data: data });
    }

    data_default(target, name, build, err) {
        target = ((target === undefined) || (target === null)) ? 0 : target;
        if (target !== 0)
            localStorage.setItem(MMTClient.storagetag, target.toString());

        return {
            vctrl: [],
            bctrl: [],
            nav: [ 0, 255 ],
            target: parseInt(target),
            config: {
                name: ((name !== undefined) && (name !== null)) ? name : "",
                build: ((build !== undefined) && (build !== null)) ? build : ""
            },
            errors: {
                what: ((err !== undefined) && (err !== null)) ? err : ""
            }
        };
    }

    is_error() {
        return ((typeof this.state.data.errors.what === "string") && (this.state.data.errors.what !== null) && (this.state.data.errors.what.trim().length > 0));
    }

    is_websocket() {
        return (this.ws && this.ws.readyState == WebSocket.OPEN);
    }

    build_ws_url() {
        if (this.state.isembed)
            return "ws://" + location.hostname + ":" + location.port + "/data";
        return 'ws://192.168.1.105:8888/data';
    }

    parse_find(data) {
        if ((this.source === undefined) ||
            (this.source === null) ||
            !Array.isArray(this.source)) return -1;

        for (var i = 0; i < this.source.length; i++) {
            if (
                (this.source[i].id === data.unit.id) &&
                (this.source[i].scene === data.unit.scene)) {
                if ((data.unit.target === 250) && ((this.source[i].target === 250) || (this.source[i].target === 251))) return i;
                else if (this.source[i].target === data.unit.target) return i;
                }
        }
        return -1;
    }

    parse_insert(data, ele) {
        if ((ele.type === 3) || (ele.type === 4))
            data.bctrl.push(ele);
        else
            data.vctrl.push(ele);
    }

    parse = (target, name, build) => {
        try {
            var src = this.source;

            if ((src !== undefined) && (src !== null) && Array.isArray(src)) {

                var data = this.data_default(target, name, build, "");
                if (data.target === 0) {
                    data.nav = this.state.data.nav;
                    this.setState({ data: data });
                    return;
                }

                src.forEach((ele) => {
                    if (ele.type !== 255) {
                        ele.invert = ((ele.type >= 4) && (ele.type <= 7));
                        if ((ele.apps === undefined) || (ele.apps === null) || !Array.isArray(ele.apps))
                            ele.apps = [];

                        if (data.target === 255) {
                            switch (ele.target) {
                                case 249:
                                case 250:
                                case 251:
                                case 252:
                                case 253:
                                case 254: break;
                                default: {
                                    this.parse_insert(data, ele);
                                    break;
                                }
                            }
                        }
                        else if (((data.target === 250) || (data.target === 251)) &&
                                 ((ele.target === 250) || (ele.target === 251))) {

                            this.parse_insert(data, ele);
                        }
                        else if (data.target === ele.target) {

                            this.parse_insert(data, ele);
                        }
                    }

                    let etarget = parseInt(ele.target);
                    switch (etarget) {
                        case 0:
                        case 249:
                        case 252:
                        case 253:
                        case 254:
                        case 255: {
                            if (data.nav.find((val) => { return val === etarget }) === undefined)
                                data.nav.push(etarget);
                            break;
                        }
                        case 250:
                        case 251: {
                            if (data.nav.find((val) => { return val === 250 }) === undefined)
                                data.nav.push(250);
                            break;
                        }
                        default: break;
                    }

                });

                if (data.nav.find((val) => { return val === data.target }) === undefined)
                    data.target = 0;
                this.setState({ data: data });
            }

        } catch (e) {
            this.catch_default(e);
        }
    };

    navigate = (target) => {
        const clz = this;
        const p = new Promise(function () {
            clz.parse(target, clz.state.data.config.name, clz.state.data.config.build);
        });
        p.catch((e) => {
            clz.catch_default(e);
        });
    };

    update = (data, issend) => {

        const clz = this;
        const p = new Promise(function () {
            const i = clz.parse_find(data);
            if ((i < 0) ||
                ((data.unit.type !== 3) && (data.unit.type !== 4) && (clz.source[i].value === data.unit.value)) ||
                (((data.unit.type === 3) || (data.unit.type === 4)) && (clz.source[i].onoff === data.unit.onoff))
            ) return;

            clz.source[i].value = data.unit.value;
            clz.source[i].onoff = data.unit.onoff;

            if (issend && (clz.ws !== null))
                clz.ws.send(JSON.stringify(data));
            clz.parse(clz.state.data.target, clz.state.data.config.name, clz.state.data.config.build);
        });
        p.catch((e) => {
            clz.catch_default(e);
        });
    };

    command = (data) => {

        const clz = this;
        const p = new Promise(function () {
            if (clz.ws !== null)
                clz.ws.send(JSON.stringify(data));
        });
        p.catch((e) => {
            clz.catch_default(e);
        });
    };

    check = () => {
        if (this.ws === undefined) return;
        const ws = this.ws;
        if ((ws === null) || (ws.readyState == WebSocket.CLOSED)) this.connect();
    };

    render() {
        return (
            <React.Fragment>
                <MMTctrls data={this.state.data} log={this.state.log} navigate={this.navigate} update={this.update} cmd={this.command} isconnected={this.is_websocket()} iserror={this.is_error()} />
            </React.Fragment>
        );
    }
}
export default MMTClient
