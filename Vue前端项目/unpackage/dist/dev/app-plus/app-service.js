if (typeof Promise !== "undefined" && !Promise.prototype.finally) {
  Promise.prototype.finally = function(callback) {
    const promise = this.constructor;
    return this.then(
      (value) => promise.resolve(callback()).then(() => value),
      (reason) => promise.resolve(callback()).then(() => {
        throw reason;
      })
    );
  };
}
;
if (typeof uni !== "undefined" && uni && uni.requireGlobal) {
  const global = uni.requireGlobal();
  ArrayBuffer = global.ArrayBuffer;
  Int8Array = global.Int8Array;
  Uint8Array = global.Uint8Array;
  Uint8ClampedArray = global.Uint8ClampedArray;
  Int16Array = global.Int16Array;
  Uint16Array = global.Uint16Array;
  Int32Array = global.Int32Array;
  Uint32Array = global.Uint32Array;
  Float32Array = global.Float32Array;
  Float64Array = global.Float64Array;
  BigInt64Array = global.BigInt64Array;
  BigUint64Array = global.BigUint64Array;
}
;
if (uni.restoreGlobal) {
  uni.restoreGlobal(Vue, weex, plus, setTimeout, clearTimeout, setInterval, clearInterval);
}
(function(vue) {
  "use strict";
  function formatAppLog(type, filename, ...args) {
    if (uni.__log__) {
      uni.__log__(type, filename, ...args);
    } else {
      console[type].apply(console, [...args, filename]);
    }
  }
  const _export_sfc = (sfc, props) => {
    const target = sfc.__vccOpts || sfc;
    for (const [key, val] of props) {
      target[key] = val;
    }
    return target;
  };
  const _sfc_main$1 = {
    data() {
      return {
        jsonlist: [],
        accessToken: "",
        temperature: null,
        // 新增温度变量
        humidity: null
        // 新增湿度变量
      };
    },
    onLoad() {
      this.getToken().then((token) => {
        this.accessToken = token.AccessToken;
        formatAppLog("log", "at pages/index/index.vue:27", "获取Token成功:", this.accessToken);
        this.startDataPolling();
      }).catch((err) => {
        formatAppLog("error", "at pages/index/index.vue:32", "获取Token失败:", err);
        this.temperature = "认证失败";
        this.humidity = "认证失败";
      });
    },
    onUnload() {
      if (this.pollingTimer) {
        clearInterval(this.pollingTimer);
      }
    },
    methods: {
      getToken() {
        return new Promise((resolve, reject) => {
          const dataInfo = {
            Account: "15511896620",
            Password: "123456"
          };
          uni.request({
            url: "http://api.nlecloud.com/Users/Login",
            method: "POST",
            data: dataInfo,
            success: (response) => {
              if (response.data && response.data.ResultObj && response.data.ResultObj.AccessToken) {
                resolve({
                  AccessToken: response.data.ResultObj.AccessToken
                });
              } else {
                reject(new Error("获取Token失败：响应格式不正确"));
              }
            },
            fail: (err) => {
              reject(err);
            }
          });
        });
      },
      // 修正：实现数据请求逻辑
      fuc_req() {
        uni.request({
          url: "http://api.nlecloud.com/devices/1331745/sensors/temperature",
          method: "GET",
          header: {
            "AccessToken": this.accessToken
          },
          success: (res) => {
            formatAppLog("log", "at pages/index/index.vue:78", res.data);
            this.temperature = res.data["ResultObj"]["Value"];
          }
        });
        uni.request({
          url: "http://api.nlecloud.com/devices/1331745/sensors/humidity",
          method: "GET",
          header: {
            "AccessToken": this.accessToken
          },
          success: (res) => {
            formatAppLog("log", "at pages/index/index.vue:89", res.data);
            this.humidity = res.data["ResultObj"]["Value"];
          }
        });
      },
      startDataPolling() {
        this.fuc_req();
        this.pollingTimer = setInterval(() => {
          this.fuc_req();
        }, 2e3);
      }
    }
  };
  function _sfc_render(_ctx, _cache, $props, $setup, $data, $options) {
    return vue.openBlock(), vue.createElementBlock("div", { class: "blur-container" }, [
      vue.createElementVNode("div", { class: "temp" }, [
        vue.createElementVNode(
          "p",
          null,
          "温度: " + vue.toDisplayString($data.temperature || "加载中...") + "℃",
          1
          /* TEXT */
        )
      ]),
      vue.createElementVNode("div", { class: "hum" }, [
        vue.createElementVNode(
          "p",
          null,
          "湿度: " + vue.toDisplayString($data.humidity || "加载中...") + "%",
          1
          /* TEXT */
        )
      ])
    ]);
  }
  const PagesIndexIndex = /* @__PURE__ */ _export_sfc(_sfc_main$1, [["render", _sfc_render], ["__scopeId", "data-v-1cf27b2a"], ["__file", "E:/vuepro/STM32智能大棚/pages/index/index.vue"]]);
  __definePage("pages/index/index", PagesIndexIndex);
  const _sfc_main = {
    onLaunch: function() {
      formatAppLog("log", "at App.vue:4", "App Launch");
    },
    onShow: function() {
      formatAppLog("log", "at App.vue:7", "App Show");
    },
    onHide: function() {
      formatAppLog("log", "at App.vue:10", "App Hide");
    }
  };
  const App = /* @__PURE__ */ _export_sfc(_sfc_main, [["__file", "E:/vuepro/STM32智能大棚/App.vue"]]);
  function createApp() {
    const app = vue.createVueApp(App);
    return {
      app
    };
  }
  const { app: __app__, Vuex: __Vuex__, Pinia: __Pinia__ } = createApp();
  uni.Vuex = __Vuex__;
  uni.Pinia = __Pinia__;
  __app__.provide("__globalStyles", __uniConfig.styles);
  __app__._component.mpType = "app";
  __app__._component.render = () => {
  };
  __app__.mount("#app");
})(Vue);
