import test, { describe } from "node:test";
import assert from "node:assert/strict";
import { createStore } from "./store.js";

describe("createStore", () => {
  test("initializes with frozen state snapshot", () => {
    const initialState = { count: 0, user: "alice" };
    const store = createStore(initialState);

    assert.deepEqual(store.getState(), { count: 0, user: "alice" });
    assert.ok(Object.isFrozen(store.getState()));
  });

  test("setState updates state and freezes next state", () => {
    const store = createStore({ count: 0, theme: "dark" });
    store.setState({ count: 1 });

    assert.deepEqual(store.getState(), { count: 1, theme: "dark" });
    assert.ok(Object.isFrozen(store.getState()));
  });

  test("setState notifies subscribers with next and previous states", () => {
    const store = createStore({ value: "initial" });
    const calls = [];

    const unsubscribe = store.subscribe((state, prevState) => {
      calls.push({ state, prevState });
    });

    store.setState({ value: "updated" });

    assert.equal(calls.length, 1);
    assert.deepEqual(calls[0].state, { value: "updated" });
    assert.deepEqual(calls[0].prevState, { value: "initial" });

    unsubscribe();
    store.setState({ value: "ignored" });
    assert.equal(calls.length, 1);
  });

  test("subscriber throwing an error does not break the notification loop", () => {
    const store = createStore({ count: 0 });
    const originalConsoleError = console.error;
    const errorLogs = [];
    console.error = (...args) => {
      errorLogs.push(args);
    };

    try {
      const callLog = [];
      const errorToThrow = new Error("Subscriber failure");

      // First subscriber succeeds
      store.subscribe((state, prevState) => {
        callLog.push({ id: 1, state, prevState });
      });

      // Second subscriber throws an error
      store.subscribe(() => {
        callLog.push({ id: 2 });
        throw errorToThrow;
      });

      // Third subscriber succeeds
      store.subscribe((state, prevState) => {
        callLog.push({ id: 3, state, prevState });
      });

      // Trigger notification
      store.setState({ count: 1 });

      // Verify all subscribers were executed in order despite error in subscriber 2
      assert.equal(callLog.length, 3);
      assert.deepEqual(callLog[0], {
        id: 1,
        state: { count: 1 },
        prevState: { count: 0 },
      });
      assert.deepEqual(callLog[1], { id: 2 });
      assert.deepEqual(callLog[2], {
        id: 3,
        state: { count: 1 },
        prevState: { count: 0 },
      });

      // Verify error was logged properly
      assert.equal(errorLogs.length, 1);
      assert.equal(errorLogs[0][0], "[KeymapStore] Subscriber exception:");
      assert.equal(errorLogs[0][1], errorToThrow);
    } finally {
      console.error = originalConsoleError;
    }
  });

  test("handles non-Error thrown objects in subscribers gracefully", () => {
    const store = createStore({ active: false });
    const originalConsoleError = console.error;
    const errorLogs = [];
    console.error = (...args) => {
      errorLogs.push(args);
    };

    try {
      let subCalled = false;

      // Subscriber throws string primitive
      store.subscribe(() => {
        throw "String error";
      });

      // Subscriber throws arbitrary object
      store.subscribe(() => {
        throw { code: 500, message: "Custom object error" };
      });

      // Subsequent subscriber runs
      store.subscribe(() => {
        subCalled = true;
      });

      store.setState({ active: true });

      assert.equal(subCalled, true);
      assert.equal(errorLogs.length, 2);
      assert.equal(errorLogs[0][0], "[KeymapStore] Subscriber exception:");
      assert.equal(errorLogs[0][1], "String error");
      assert.equal(errorLogs[1][0], "[KeymapStore] Subscriber exception:");
      assert.deepEqual(errorLogs[1][1], {
        code: 500,
        message: "Custom object error",
      });
    } finally {
      console.error = originalConsoleError;
    }
  });
});
