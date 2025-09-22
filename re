local ReplicatedStorage = game:GetService("ReplicatedStorage")
local Players = game:GetService("Players")
local LocalPlayer = Players.LocalPlayer

-- 🔎 Find the recycler remote dynamically
local function getRecyclerRemote()
    for _, obj in ipairs(ReplicatedStorage:WaitForChild("API"):GetChildren()) do
        if obj:IsA("RemoteFunction") then
            return obj
        end
    end
end

-- 🔎 Find the claim remote dynamically
local function getClaimRemote()
    for _, obj in ipairs(ReplicatedStorage:WaitForChild("API"):GetChildren()) do
        if obj:IsA("RemoteFunction") and obj.Name ~= getRecyclerRemote().Name then
            return obj
        end
    end
end

-- ♻️ Send recycler request
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

-- ✅ Send claim request
local function claim(remote)
    local args = {
        "red_panda",
        "beaver",
        "buffalo",
        "aztec_egg_2025_ahuizotl",
        "aztec_egg_2025_jungle_frog",
        "chocolate_labrador",
        "cat",
        "snow_cat",
        "puma",
        "basic_egg_2022_ant",
        "fennec",
        "otter",
        "beach_2024_mahi_mahi",
        "camping_2025_muskrat",
        "basic_egg_2022_mouse",
        "ginger_cat",
        "basic_egg_2022_donkey",
        "star_rewards_2022_pomeranian",
        "basic_egg_2022_camel",
        "basic_egg_2022_parakeet",
        "aztec_egg_2025_tegu",
        "aztec_egg_2025_ehecatl",
        "aztec_egg_2025_xiucohtl",
        "aztec_egg_2025_golden_lynx"
    }
    return remote:InvokeServer(unpack(args))
end

-- 🔄 Auto recycler + auto claim loop
task.spawn(function()
    local recyclerRemote = getRecyclerRemote()
    if not recyclerRemote then
        warn("⚠️ Recycler remote not found")
        return
    end

    local claimRemote = getClaimRemote()
    if not claimRemote then
        warn("⚠️ Claim remote not found")
        return
    end

    while true do
        -- Recycle pets or points
        local uniques = {} -- empty table → recycle points if no pets
        sendRecycler(recyclerRemote, uniques)
        print("♻️ Recycled pets/points")

        -- Claim rewards
        claim(claimRemote)
        print("🎁 Claimed rewards")

        task.wait(2) -- prevent double-fire
    end
end)
