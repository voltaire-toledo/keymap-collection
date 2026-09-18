/* ======================================================================
   VT Mello Keymaps - Centralized Application State Store
   Predictable, Unidirectional Reactive State Container
   ====================================================================== */

/**
 * @typedef {Object} KeyCoordinate
 * @property {number} row
 * @property {number} col
 * @property {number|undefined} [sub]
 */

/**
 * @typedef {Object} AppState
 * @property {string} keyboardId
 * @property {string} layerId
 * @property {string} releaseId
 * @property {string} colorwayId
 * @property {string} theme
 * @property {KeyCoordinate|null} selectedCoord
 * @property {Object|null} selectedCell
 * @property {KeyCoordinate|null} hoveredCoord
 * @property {Object|null} hoveredCell
 */

/**
 * Creates an observable store instance.
 * @param {AppState} initialState
 */
export function createStore(initialState) {
  let state = Object.freeze({ ...initialState });
  const listeners = new Set();

  return {
    /**
     * Retrieve a snapshot of the current state.
     * @returns {Readonly<AppState>}
     */
    getState() {
      return state;
    },

    /**
     * Atomically transition state and notify subscribers of changes.
     * @param {Partial<AppState>} updates
     */
    setState(updates) {
      const nextState = Object.freeze({ ...state, ...updates });
      if (nextState === state) return;

      const prevState = state;
      state = nextState;

      for (const listener of listeners) {
        try {
          listener(state, prevState);
        } catch (err) {
          console.error("[KeymapStore] Subscriber exception:", err);
        }
      }
    },

    /**
     * Subscribe to state change notifications.
     * @param {(state: Readonly<AppState>, prevState: Readonly<AppState>) => void} listener
     * @returns {() => void} Unsubscribe function
     */
    subscribe(listener) {
      listeners.add(listener);
      return () => listeners.delete(listener);
    },
  };
}
