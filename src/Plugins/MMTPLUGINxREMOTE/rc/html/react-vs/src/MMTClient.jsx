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
            onoff: props.unit.onoff ? true : false
        };
        this.handleClick = this.handleClick.bind(this);
    }
    static getDerivedStateFromProps(props, state) {
        if ((props.id === state.id) && (props.unit.onoff !== state.onoff)) {
            return {
                onoff: props.unit.onoff
            };
        }
        return null;
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
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    render() {
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
                    <div className="labeled">
                        <span className="labeled-span">{`${this.props.unit.id}/${this.props.unit.scene}`}</span>
                        <div className="labeled-content" >
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
                    <div className="labeled">
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
                        min="0" max="127" step="1" value={this.state.value} list="l1" />
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
        this.url = "https://claudiacoord.github.io/MIDI-MT";
        this.get_key = this.get_unit_key.bind(this);
        this.get_href_key = this.get_href_key.bind(this);
        this.handleClickFS = this.handleClickFS.bind(this);
        this.isfs = false;
    }
    componentDidCatch(err, info) {
        console.log(err, info.componentStack);
    }
    get_unit_key(u) {
        return (u.scene * 1000000) + (u.id * 1000) + u.longtarget;
    }
    get_href_key(i) {
        return (this.props.data.target * 1000) + (i * 100);
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
    render() {
        if (
            ((this.props.data === undefined) || (this.props.data === null)) ||
            ((this.props.data.vctrl.length === 0) && (this.props.data.bctrl.length === 0) && (this.props.data.nav.length === 2)))
            return (
                <React.Fragment key="10001">
                    <nav>
                        <div className="loader"></div>
                        <a className="right-menu" href={this.url} target="_blank" rel="noreferrer">MIDI-MT</a>
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
                            <MMTnav target={target} key={this.get_href_key(index)} actived={(this.props.data.target === target)} navigate={this.props.navigate} />
                        )}
                        <a className="right-menu" href={this.url} target="_blank" rel="noreferrer">MIDI-MT</a>
                        <a className={`right-btn ${(this.state.isfs ? 'icon-ns' : 'icon-fs')}`} href="#" onClick={() => this.handleClickFS()}>&nbsp;</a>
                    </nav>
                    <br />
                    <h3 className="h3-top-level">About</h3>
                    <div className="awaiting-line">
                        <br />You are in the <b>MIDI-MT</b> remote control app.
                        <br />Manage all the controls that are described in the <b>MIDI-MT</b> configuration.
                        <br /><b>MIDI-MT</b> must be configured and running.
                    </div>
                    <br />
                    <br />
                    <h3 className="h3-next-level h3-warning" style={{ visibility: (this.props.iserror) ? 'visible' : 'hidden' }}>Warning!</h3>
                    <div className="awaiting-error" style={{ visibility: (this.props.iserror) ? 'visible' : 'hidden' }}>
                        <p><span>Info:</span>&nbsp;{this.props.data.errors.what}</p>
                    </div>
                </React.Fragment>
            );
        else
            return (
                <React.Fragment>
                    <nav>
                        {this.props.data.nav.map((target, index) =>
                            <MMTnav target={target} key={this.get_href_key(index)} actived={(this.props.data.target === target)} navigate={this.props.navigate} />
                        )}
                        <a className="right-menu" href={this.url} target="_blank" rel="noreferrer">MIDI-MT</a>
                        <a className={`right-btn ${(this.state.isfs ? 'icon-ns' : 'icon-fs')}`} href="#" onClick={() => this.handleClickFS()}>&nbsp;</a>
                    </nav>
                    <br/>
                    <h3 className="h3-top-level">{Named.get(this.props.data.target)}</h3>
                    <form>
                        <div className="scrollable-v" style={{ visibility: (this.props.data.vctrl.length === 0) ? 'hidden' : 'visible' }}>
                            {this.props.data.vctrl.map((unit) => {
                                const k = this.get_unit_key(unit);
                                return <MMTvctrl key={k} id={k} unit={unit} update={this.props.update} />
                            })}
                            <datalist id="l1">
                                <option value="0" label="0"></option>
                                <option value="64" label="50"></option>
                                <option value="127" label="100"></option>
                            </datalist>
                        </div>
                    </form>
                    <form>
                        <div className="scrollable-b" style={{ visibility: (this.props.data.bctrl.length === 0) ? 'hidden' : 'visible' }}>
                            {this.props.data.bctrl.map((unit) => {
                                const k = this.get_unit_key(unit);
                                return <MMTbctrl key={k} id={k} unit={unit} update={this.props.update} />
                            })}
                        </div>
                    </form>
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
    constructor(props) {
        super(props);
        this.state = {
            data: this.data_default(0, "", "", ""),
            isembed: ((props.isembed !== undefined) && (props.isembed !== null)) ? props.isembed : false
        };
        this.ws = null;
        this.wsonce = false;
        this.source = [];
        this.update = this.update.bind(this);
        this.parse = this.parse.bind(this);
        this.parse_find = this.parse_find.bind(this);
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
                            let target = parseInt(localStorage.getItem("target"));
                            clz.parse(target, d.config.name, d.config.build);
                        });
                        p.catch((e) => {
                            clz.catch_default(e);
                        });

                    } else if ((d.action === "changed") && (this.source !== undefined) && (this.source !== null) && Array.isArray(this.source)) {

                        clz.update(d, false);
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
            localStorage.setItem("target", target.toString());

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
                (this.source[i].scene === data.unit.scene) &&
                (this.source[i].target === data.unit.target)) return i;
        }
        return -1;
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
                                    if ((ele.type === 3) || (ele.type === 4))
                                        data.bctrl.push(ele);
                                    else
                                        data.vctrl.push(ele);
                                    break;
                                }
                            }
                        }
                        else if (data.target === ele.target) {
                            if ((ele.type === 3) || (ele.type === 4))
                                data.bctrl.push(ele);
                            else
                                data.vctrl.push(ele);
                        }
                    }

                    let etarget = parseInt(ele.target);
                    switch (etarget) {
                        case 0:
                        case 249:
                        case 250:
                        case 251:
                        case 252:
                        case 253:
                        case 254:
                        case 255: {
                            if (data.nav.find((val) => { return val === etarget }) === undefined)
                                data.nav.push(etarget);
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

    check = () => {
        if (this.ws === undefined) return;
        const ws = this.ws;
        if ((ws === null) || (ws.readyState == WebSocket.CLOSED)) this.connect();
    };

    render() {
        return (
            <React.Fragment>
                <MMTctrls data={this.state.data} navigate={this.navigate} update={this.update} isconnected={this.is_websocket()} iserror={this.is_error()} />
            </React.Fragment>
        );
    }
}
export default MMTClient
