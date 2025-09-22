local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

local API = ReplicatedStorage:WaitForChild("API")

-- 🔎 auto-detect recycler RemoteFunction
local function getRecyclerRemote()
    for _, obj in ipairs(API:GetChildren()) do
        if obj:IsA("RemoteFunction") then
            return obj
        end
    end
    return nil
end

-- 🐾 helper: send recycle request
local function recyclePets(remote, petUniques)
    local args = {
        "f-27",
        "UseBlock",
        {
            action = "use",
            uniques = petUniques or {}
        },
        LocalPlayer.Character
    }
    local result = remote:InvokeServer(unpack(args))
    return result
end

-- ♻️ loop forever
task.spawn(function()
    while task.wait(1) do
        local recyclerRemote = getRecyclerRemote()
        if not recyclerRemote then
            warn("⚠️ Recycler remote not found, retrying...")
            continue
        end

        -- 🧾 fetch inventory
        local clientData = ReplicatedStorage:FindFirstChild("ClientData")
        if not clientData then
            warn("⚠️ ClientData missing")
            continue
        end

        local inventory = require(clientData).get("inventory")
        local pets = inventory and inventory.pets or {}

        -- 🎯 pick commons → ultras
        local petsToRecycle = {}
        for unique, pet in pairs(pets) do
            if pet.rarity == "Common" or pet.rarity == "Uncommon" or pet.rarity == "Rare" or pet.rarity == "Ultra" then
                table.insert(petsToRecycle, unique)
            end
        end

        if #petsToRecycle == 0 then
            print("✅ No pets to recycle, claiming leftover rewards...")
            recyclePets(recyclerRemote, {}) -- claim leftover points
            task.wait(5) -- wait before checking again
        else
            print("♻️ Recycling", #petsToRecycle, "pets...")
            local BATCH_SIZE = 20
            local index = 1
            while index <= #petsToRecycle do
                local batch = {}
                for i = index, math.min(index + BATCH_SIZE - 1, #petsToRecycle) do
                    table.insert(batch, petsToRecycle[i])
                end
                recyclePets(recyclerRemote, batch)
                index += BATCH_SIZE
                task.wait(2) -- cooldown safety
            end
        end
    end
end)
