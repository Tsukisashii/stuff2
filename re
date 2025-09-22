local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer
local Workspace = game:GetService("Workspace")

-- 🔎 find recycler remote
local function getRecyclerRemote()
    for _, obj in ipairs(ReplicatedStorage:WaitForChild("API"):GetChildren()) do
        if obj:IsA("RemoteFunction") then
            return obj
        end
    end
end

-- ♻️ send recycler
local function sendRecycler(remote, uniques)
    local args = {
        "f-27",
        "UseBlock",
        {
            action = "use",
            uniques = uniques or {}
        },
        LocalPlayer.Character
    }
    return remote:InvokeServer(unpack(args))
end

-- 🐾 get all pets from InventoryDB
local InventoryDB = require(ReplicatedStorage:WaitForChild("InventoryDB"))
local function getPetsToRecycle()
    local list = {}
    for unique, pet in pairs(InventoryDB.pets or {}) do
        local rarity = pet.rarity
        if rarity == "Common" or rarity == "Uncommon" or rarity == "Rare" or rarity == "Ultra" then
            table.insert(list, unique)
        end
    end
    return list
end

-- ⏱ find in-game recycler timer label
local function getTimerLabel()
    local furniture = Workspace:WaitForChild("HouseInteriors")
        :FindFirstChild("furniture", true)
    if furniture then
        local gui = furniture:FindFirstChildWhichIsA("SurfaceGui", true)
        if gui then
            return gui:FindFirstChildWhichIsA("TextLabel", true)
        end
    end
end

-- 🔄 main auto-recycler loop with timer check
task.spawn(function()
    local recyclerRemote = getRecyclerRemote()
    if not recyclerRemote then
        warn("⚠️ Recycler remote not found")
        return
    end

    local timerLabel = getTimerLabel()
    if not timerLabel then
        warn("⚠️ Couldn’t find recycler timer label")
        return
    end

    while true do
        -- wait until "READY"
        repeat task.wait(1) until timerLabel.Text == "READY"

        local petsToRecycle = getPetsToRecycle()
        if #petsToRecycle > 0 then
            print("♻️ Recycling...", #petsToRecycle, "pets")
            sendRecycler(recyclerRemote, petsToRecycle)
        else
            print("♻️ No eligible pets. Recycling points only if available...")
            sendRecycler(recyclerRemote, {})
        end

        -- short cooldown so we don’t double-fire
        task.wait(2)
    end
end)
